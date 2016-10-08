/*********************************************
* The Producer-Consumer Problem
* Author : Chauncey Yan,
*          Wang Xiaomei,
*          Guo 
* Goal   : Producers create items of some kind and 
*          add them to a data structure; consumers 
*          remove the items and process them.
**********************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include "mt19937ar.h"
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

static pthread_mutex_t mutex_sum;

extern void *print1();
extern void *printNumbers();

sem_t *s1;
sem_t *s2;

unsigned long rnd_int()
{
    if (check_rd() == 1 ){ //rdrand()
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
int check_rd()
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
    if ((ecx>>30)&0x01 == 1)
        return 1;

    return 0;
}

void *print1()
{
    do {
        printf("hello ");
        printf("world\n");
    }
    while(1);
}
void *printNumbers()
{
    do {
        // sem_wait(sem1);
        int seed;
        init_genrand((unsigned long)&seed);
        sleep(1);
        sem_trywait(s1);
        printf("%d\n",rnd_int());
        fflush(stdout);
    }
    while (1);
}

int main(int argc, char *argv[], char *envp[]) 
{
    char *c = "semaphore 1";
    //char *b = "semaphore 2";
    int s;

    s1 = sem_open(c, O_CREAT, 0666, 0);
    //s2 = sem_open(b, O_CREAT, 0666, 0);
    pthread_t *threads;

    threads = (pthread_t *) malloc(100 * sizeof(pthread_t));
    pthread_create(&threads[0], NULL,print1,NULL);
    for (s = 1;s < 100;s++) {
            pthread_create(&threads[s], NULL,printNumbers,NULL);
            }
            for (s = 0; s < 100; s++) {
            pthread_join(threads[s], NULL);
            }
            }

