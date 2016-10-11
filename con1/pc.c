/**************************************************
 * Producer-Consumer Problem
 * Author : Chauncey Yan,
 *          Wang Xiaomei,
 *          Guo Xilun
 * Goal   : Producers create items of some kind and
 *          add them to a data structure; consumers
 *          remove the items and process them.
 * Feature: Producer produce once per 3-7s
 *          Producer create a random number
 *          producer create a random second between
 *          2-9s for consumer to processing 
 *          Consumer display the last number in buf
 *          Consumer process(sleep) upon access buf
 **************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include "mt19937ar.c"

static pthread_mutex_t mutex_sum;

extern void *consumer_thread(void *a);
extern void *producer_thread(void *a);

sem_t *s1;
sem_t *s2;

struct t_elements {
    int num;
    int sec;

};

int rd_rst;
unsigned long program_start, elapse;
struct t_elements buffer[32];

unsigned long rd_int()
{
    if (rd_rst == 1 ){ //rdrand()
        unsigned long ul = 0ul;
        __asm__ __volatile__(
                "rdrand %0"
                :"=r"(ul)
                );
        return ul;
    } else {
        return genrand_int32();

    }
}

void check_rd()
{
    unsigned long eax = 1, ecx,ebx;
    __asm__ __volatile__(
            "cpuid"
            :"=a"(eax),
            "=b"(ebx),
            "=c"(ecx)
            :"0"(eax),
            "2"(ecx)
            );
    if ((ecx>>30)&0x01 == 1){
        printf("Intel chip detected! Using rdrand().\n");
        rd_rst = 1;
    }
    printf("Intel chip not detected! Using Mersenne Twister.\n");
    rd_rst = 0;
}
/*****************************************
 * Print out the buffer every second
 *****************************************/
void printbuf(){
    for(int i=0;i<65;i++)printf("-");
    printf("\n|");
    for(int i=0;i<32;i++){
        if (buffer[i].num == 0) printf(" |");
        else printf("%c|",buffer[i].num);
    }
    printf("\n");
    for(int i=0;i<65;i++)printf("-");
    printf("\n|");
    for(int i=0;i<32;i++)printf("%d|",buffer[i].sec);
    printf("\n");
    for(int i=0;i<65;i++)printf("-");
    printf("\n");
}

void *consumer_thread(void *a)
{
    do {
        int s_time = 0;
        int z = 0;
        int tid = pthread_self();
        
        if (buffer[0].num == NULL) { //consumers wait if buffer is empty
            sem_wait(s2);
        }
        
        for (z = 31; z >= 0; z--) {
            if (buffer[z].num != NULL) {
                //sleep(buffer[z].sec); //consumer sleep with the sec producer create
                pthread_mutex_lock(&mutex_sum);
                elapse = (unsigned)time(NULL)-program_start;

                printf("\nTime elapsed %ds\n", elapse);
                printf("Consumer%d Consumed @ buf[%d]\n", tid, z);
                printf("Consumer%d Consumed char   : %c\n", tid, buffer[z].num);
                printf("Consumer%d Consumed second : %d\n", tid, buffer[z].sec);
                
                s_time = buffer[z].sec;
                
                buffer[z].num = NULL;//consuming
                buffer[z].sec = NULL;

                printbuf();
                 
                sem_post(s1);
                pthread_mutex_unlock(&mutex_sum);
                
                sleep(s_time); //consumer sleep with the sec producer create
                break;
            }
        }
    }
    while (1);
}

void *producer_thread(void *a)
{
    do {
        int j = 0;
        int tid = pthread_self();
        sleep(rd_int()%5+3);    // 3-7s 
        //sleep(2);
        if (buffer[31].sec != NULL) {
            sem_wait(s1);
        }
        
        pthread_mutex_lock(&mutex_sum);

        for (j = 0; j < 32; j++) {
            if (buffer[j].num == NULL) {
                buffer[j].num = rd_int() % 25 + 65; // A-Z
                buffer[j].sec = rd_int() % 8 + 2;   // 2-9s 
                //buffer[j].sec = 4;
                
                elapse = (unsigned)time(NULL)-program_start;
   
                printf("\nTime elapsed %ds\n", elapse);
                printf("Producer%d produced @ buf[%d]\n", tid, j);
                printf("Producer%d Produced char   : %c\n", tid, buffer[j].num);
                printf("Producer%d Produced second : %d\n", tid, buffer[j].sec);
                printbuf();
                
                sem_post(s2);
                break;
            }
         }
         pthread_mutex_unlock(&mutex_sum);

    }
    while (1);
}

int main(int argc, char *argv[], char *envp[]) 
{
    program_start = (unsigned)time(NULL);
    char *c = "semaphore 1";
    char *b = "semaphore 2";

    s1 = sem_open(c, O_CREAT, 0666, 0);
    s2 = sem_open(b, O_CREAT, 0666, 0);
    int proN = atoi(argv[1]);
    int conN = atoi(argv[2]);
    int s = 0;
    int z = 0;
    int seed=time(NULL);
    check_rd();
    init_genrand((unsigned long)&seed);
    pthread_t *threads;

    threads = (pthread_t *) malloc((proN + conN) * sizeof(pthread_t));
    pthread_mutex_init(&mutex_sum, NULL);
    for (s = 0, z = 0; (s + z) < (proN + conN);) {
        if (s < proN) {
            pthread_create(&threads[s], NULL, producer_thread,NULL);
            s++;
        }
        if (z < conN ) {
            pthread_create(&threads[proN + z], NULL, consumer_thread,NULL);
            z++;
        }
    }
    for (s = 0; s < (proN+conN); s++) {
        pthread_join(threads[s], NULL);
    }
}

