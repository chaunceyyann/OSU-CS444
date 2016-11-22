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

typedef enum{ INSERT, SEARCH, DELETE } checker_t;
typedef enum{ ON, OFF } state;
sem_t sem_search;
sem_t sem_delete;
sem_t sem_insert;
state state_delete = OFF;
state state_insert = OFF;
state state_search = OFF;
int search_count = 0;
int insert_count = 0;
int delete_count = 0;
int gc;                 // global counter

// singly linked list struct
struct node {
    int val;
    struct node *next;
};
// declarations
void sll_print(struct node *);
void sll_search(struct node *);
void sll_insert(struct node *);
void sll_del(struct node *);

/* *******************************************
 * Main
 * ******************************************/
int main(){
    int i,seed;
    init_genrand((unsigned long)&seed);
    struct node *head;
    head = malloc(sizeof(struct node));
    head->val = genrand_int32()%20;

    // int sem_init(sem_t *sem, int pshared, unsigned int value)
    sem_init(&sem_search, 0, 0);
    sem_init(&sem_insert, 0, 0);
    sem_init(&sem_delete, 0, 0);

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

    /* exit */  
    exit(0);
}

/* *******************************************
 * check function
 ********************************************/
void check(checker_t checker) { 
    switch (checker) {
        case INSERT:
            if (state_delete == OFF && state_insert == OFF) {   /* Inserter can only work with search. */
                sem_post(&sem_insert);
            }
            break;

        case SEARCH:                /* Searcher can work parallelly with search and insert, but cannot work with delete. */
            if (state_delete == OFF) {	
                sem_post(&sem_search);
            }
            break;

        case DELETE:                /* Deleter can work with nobody. */
            if (state_insert == OFF && search_count == 0) {
                sem_post(&sem_delete);
            }
            break;
    }
}

/* *******************************************
 * Searchers merely examine the list; 
 * hence they can execute concurrently 
 * with each other. Inserters add new 
 * items to the end of the list; 
 * ******************************************/
void sll_search(struct node * list){ // list === head in main
    int c = 0, i, rdn, rt=3;
    struct node * cursor;
    while (1){
        if(state_delete == OFF) {
            sem_post(&sem_search);
        }
        else
            printf("Searcher is waiting on deleter.\n");

        sem_wait(&sem_search);
        search_count++; 
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
            usleep(500000);
        }
        search_count--; 
        check(DELETE);
        usleep(600000);
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
void sll_insert(struct node * list){
    int c = 0, i, rdn, rt=1;
    struct node * cursor;
    for (i=0;i<10;i++){
        if (state_insert == OFF && state_delete == OFF)
        {	
            sem_post(&sem_insert);
        }
        else
            printf("Inserter is waiting on other inserter or deleter.\n");
        sem_wait(&sem_insert);
        state_insert = ON;
        insert_count++;
        rdn = genrand_int32()%20;
        cursor = list;
        //printf("top of the list:%d\n", list->val);
        if (cursor != 0){
            while(cursor->next != NULL){
                cursor = cursor->next;
            }
            cursor->next = malloc(sizeof(struct node));
            cursor->next->val = rdn;
            printf("[Insert %d]\n", rdn);
        } else {
            printf("List is null\n");
            cursor = malloc(sizeof(struct node));
            cursor->val = rdn;
            cursor->next = NULL;
            printf("[Insert %d]\n", rdn);
        }
        sll_print(list);
        insert_count--;
        state_insert = OFF;
        check(DELETE);
        check(INSERT);
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
void sll_del(struct node * list){
    int cnt = 0,i, rdn;
    int rt=6; 
    struct node * cursor, * prev, *tmp;
    while (1) {
        sleep(2);
        if(search_count == 0 && state_insert == OFF) {
            sem_post(&sem_delete);
        }
        else 
            printf("Deleter is waiting on other inserter or searcher.\n");

        sem_wait(&sem_delete);	
        state_delete = ON;
        delete_count++; 

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
            rdn = genrand_int32()%cnt;
            printf("cnt:%d\n",cnt);
            cnt = 0;
            if (rdn != 0) {             // check if we need to delete top
                printf("rdn:%d\n",rdn);
                for (i=0; i<rdn; ++i) { // move the cursor to the position
                    prev = cursor;
                    cursor = cursor->next;
                }
                printf("[Deleting %d @ %d]\n",cursor->val,++rdn);
                prev->next = cursor->next;
                //free(cursor);
                //cursor = prev->next;
            }
            else {
                if (list->next == 0) {
                    if (list != 0) {
                        printf("[Deleting %d].\nList is empty, for now.\n",list->val,++rdn);
                        free(list);
                        list = 0;
                    }
                    else 
                        printf("List is empty, for now.\n");
                } 
                else {
                    printf("[Deleting %d @ %d]\n",list->val,++rdn);
                    list = list->next;
                    cursor = NULL;
                    //free(cursor);
                    cursor = list;
                }
            }
        }       

        delete_count--;
        state_delete = OFF;
        sll_print(list);
        check(INSERT);
        check(SEARCH);
    }
    pthread_exit(&rt);
}

/* *******************************************
 * print all items in the list
 * ******************************************/
void sll_print(struct node * list){
    int rt=8,cnt=0,i;
    printf("************************************************************************\n");
    struct node * cursor;
    cursor = list;
    if (cursor != 0){
        while (cursor!=NULL) {
            ++cnt;
            cursor = cursor->next;
        }
        printf("cnt:%d\n",cnt);
        cursor = list;
        for (i=0;i<cnt;++i) {
            printf(" %d ",cursor->val);
            cursor = cursor->next;
        }
    } else {
        printf("List is null, exit now.\n");
        exit(0);
    }
    printf("\n************************************************************************\n\n");
}
