/* ******************************************
 * Program  : concurrency practice 3
 * Objective: Three kinds of threads share 
 * access to a singly-linked list: 
 * searchers, inserters and deleters. 
 * Author   : Chauncey Yan, Arvind Vepa
 * Date     : May 8th
 * ******************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
#include<errno.h>
#include"mt19937ar.h"

sem_t sem1;             // semaphore 1 for insert
sem_t sem2;             // for search
sem_t sem3;             // for del
sem_t sem0;             // for print
int gc;                 // global counter

// singly linked list struct
struct node {
        int val;
        struct node *next;
};
// declarations
void sll_print(struct node *);
void *sll_search(struct node *);
void *sll_insert(struct node *);
void *sll_del(struct node *);

/* *******************************************
 * Main
 * ******************************************/
int main(){
        int i,seed;
        init_genrand((unsigned long)&seed);
        struct node *head;
        head = malloc(sizeof(struct node));
        
        // int sem_init(sem_t *sem, int pshared, unsigned int value)
        // initialize sem to 1 - binary semaphore
        sem_init(&sem1,0,1);    // semaphore for searcher mutually exclusive
        sem_init(&sem2,0,1);    // semaphore for inserter mutually exclusive
        sem_init(&sem3,0,1);    // semaphore for deleter mutually exclusive
        //sem_init(&sem0,0,1);

        pthread_t thread[6];    // 2 search 2 insert 1 delete
        pthread_create(&thread[0], NULL, (void *) &sll_search, (void *) head);
        pthread_create(&thread[1], NULL, (void *) &sll_search, (void *) head);
        pthread_create(&thread[2], NULL, (void *) &sll_insert, (void *) head);
        pthread_create(&thread[3], NULL, (void *) &sll_insert, (void *) head);
        pthread_create(&thread[4], NULL, (void *) &sll_del, (void *) head);
        //pthread_create(&thread[5], NULL, (void *) &sll_del, (void *) head);
        pthread_join(thread[0],NULL);
        pthread_join(thread[1],NULL);
        pthread_join(thread[2],NULL);
        pthread_join(thread[3],NULL);
        pthread_join(thread[4],NULL);
        //pthread_join(thread[5],NULL);

        sem_destroy(&sem1); /* destroy semaphore */
        sem_destroy(&sem2); /* destroy semaphore */
        sem_destroy(&sem3); /* destroy semaphore */
        sem_destroy(&sem0); /* destroy semaphore */
                  
        /* exit */  
        exit(0);
}



/* *******************************************
 * Searchers merely examine the list; 
 * hence they can execute concurrently 
 * with each other. Inserters add new 
 * items to the end of the list; 
 * ******************************************/
void *sll_search(struct node * list){ // list === head in main
        int c = 0, i, rdn, rt=3;
        struct node * cursor;
        while (1){
                sem_wait(&sem1);
                rdn = genrand_int32()%20;
                cursor = list;
                if (cursor != 0){
                        do {
                                if (cursor->val != rdn){
                                        cursor = cursor->next;
                                } else {
                                        printf("[Searcher found %d]\n",rdn);
                                        break;          // only find one
                                }
                        } while (cursor != 0);
                } else {
                        printf("List is null");
                        sleep(1);
                }
                sem_post(&sem1);
                sleep(1);
        }
        pthread_exit(&rt);
}

/* ****************************************** 
    pthread_create (&thread_b, NULL, (void *) &handler, (void *) &i[1]);
 * insertions must be mutually exclusive 
 * to preclude two inserters from 
 * inserting new items at about the same 
 * time. However, one insert can proceed 
 * in parallel with any number of searches. 
                sem_getvalue(&sem_name, &value); 
                printf("The value of the semaphors is %d\n", value);
 * ******************************************/
void *sll_insert(struct node * list){
        int c = 0, i, rdn, rt=1;
        struct node * cursor;
        struct node * temp;
        for (i=0;i<5;i++){
                printf("insert-sem2:%d\n",sem_wait(&sem2));
                printf("deleter-sem3:%d\n",sem_wait(&sem3));
                rdn = genrand_int32()%20;
                cursor = list;
                printf("top of the list:%d\n", list->val);
                if (cursor->next != NULL){
                        while(cursor->next != NULL){
                                cursor = cursor->next;
                        }
                } else {
                    printf("List is null\n");
                }
                printf("[Insert %d]\n", rdn);
                cursor->val = rdn;
                printf("test\n");
                cursor->next = malloc(sizeof(struct node));
                cursor = cursor->next;

                sll_print(list);
                sem_post(&sem3);
                sem_post(&sem2);
                sleep(1);
        }
        pthread_exit(&rt);
}

/* *******************************************
 * Finally, deleters remove items from 
 * anywhere in the list. At most one 
 * deleter process can access the list at a
 * time, and deletion must also be mutually 
 * exclusive with searches and insertions.
 * ******************************************/
void *sll_del(struct node * list){
    int cnt = 0,i, rdn;
    int rt=6; 
    struct node * cursor, * prev;
    sleep(1);
    printf("deleter-sem3:%d\n",sem_wait(&sem3));
    // printf("inserter-sem2:%d\n",sem_wait(&sem2));
    // count the length of the linked list
    cursor = list;
    while (cursor!=NULL) {
        ++cnt;
        cursor = cursor->next;
    }
    if (cnt == 0) {
        printf("List is null, not deleting.");
    }
    else {
        // generate a random number and delete that node
        cursor = list;
        int seed;
        init_genrand((unsigned long)&seed);
        //rdn = genrand_int32()%cnt;
        rdn = 1;
        if (rdn != 0) {             // check if we need to delete top
            for (i=0; i<rdn; ++i) { // move the cursor to the position
                prev = cursor;
                cursor = cursor->next;
            }
            printf("[Deleting %d @ %d]\n",cursor->val,++rdn);
            prev->next = cursor->next;
            free(cursor);
            cursor = prev->next;
        }
        else {
            list = list->next;
            free(cursor);
            printf("[Deleting %d @ %d]\n",list->val,++rdn);
        }


    }       
            
    sll_print(list);
    sem_post(&sem3);
    sem_post(&sem1);
    sem_post(&sem2);
    pthread_exit(&rt);
}

                
        
/* *******************************************
 * Finally, deleters remove items from 
 * anywhere in the list. At most one 
 * deleter process can access the list at a
 * time, and deletion must also be mutually 
 * exclusive with searches and insertions.
 * ******************************************/
//void sll_del(struct node * list){
//        int c = 0,i, rdn;
//        int rt=6; 
//        struct node * cursor, * prev;
//        sleep(3);
//        while (1){
//                printf("deleter-sem3:%d\n",sem_wait(&sem3));
//               // printf("inserter-sem2:%d\n",sem_wait(&sem2));
//                c = 0;
//                cursor = list;
//                len = list.capbility;
//                if (cursor != 0){
//                        do {
//                                if (cursor->val != rdn){
//                                        prev = cursor;
//                                        cursor = cursor->next;
//                                } else {
//                                        printf("Delete %d\n", list->val);
//                                        if (c == 0){ // delete from top
//                                                if (list->next != 0)
//                                                        list = list->next;
//                                                free(cursor);
//                                        } else {
//                                                if (cursor->next != 0){
//                                                        prev->next = cursor->next;
//                                                        free(cursor);
//                                                        cursor = prev->next;
//                                                } else { 
//                                                        cursor=NULL;
//                                                        free(cursor);
//                                                }
//                                        }
//                                }
//                                c = 1;
//                        } while (cursor != 0);
//                        sll_print(list);
//                        sem_post(&sem3);
//                        sem_post(&sem1);
//                        sem_post(&sem2);
//                        usleep(1000000);
//
//                } else {
//                        printf("List is null");
//                }
//        }
//        pthread_exit(&rt);
//}

/* *******************************************
 * print all items in the list
 * ******************************************/
void sll_print(struct node * list){
        int rt=8;
        //sem_wait(&sem1);
        printf("************************************************************************\n");
        struct node * cursor;
        cursor = list;
        if (cursor != 0){
                do {
                        printf(" %d ",cursor->val);
                        cursor = cursor->next;
                } while (cursor->next != 0);
        } else {
                printf("List is null");
                exit(0);
        }
        printf("\n************************************************************************\n");
        //sem_post(&sem3);
        //pthread_exit(&rt);
}
