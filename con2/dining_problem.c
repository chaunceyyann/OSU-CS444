/*****************************************************************************
 * Concurrenc problem 2 
 * 5 Philosophers have a dinner with 5 forks. They have to share their forks
 * to eat their plate. Who dont have 2 forks has to wait/think before eat.
 * Group 15
 * Chauncey Yan
 * Xiaomei Wang
 * Xilun Guo
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include "mt19937ar.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

static pthread_mutex_t mutex_sum;

static void signal_handler(int);
static void onexit_function(void);
static sig_atomic_t signal_received = 0;

extern void * dinersProblem(int *a);
// 0 == not in use, 1 == in use 
int forks[5]={0,0,0,0,0};
// 0 == thinking, 1 == eating
int philosophers[5];
int eat_count[5]={0,0,0,0,0};
const char *name[]={"0:Aristotle ","1:Kant      ","2:Nietzsche ",
                    "3:Plato     ","4:Socrates  "};
const char *stage[]={"think","eat"};

int bowls[5];

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
unsigned long rdn_int()
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

void print_table()
{
        pthread_mutex_lock(&mutex_sum);
        int i;
        printf("***************************************************\n");
        for (i=0;i<5;i++)
                printf("%s is %sing.\n",name[i],stage[philosophers[i]]);
        printf("phi  :");
        for (i=0;i<5;i++)
                printf(" %3d",philosophers[i]);
        printf("\n");

        printf("fork :");
        for (i=0;i<5;i++)
                printf(" %3d",forks[i]);
        printf("\n");
        printf("count:");
        for (i=0;i<5;i++)
                printf(" %3d",eat_count[i]);
        printf("\n");
        printf("***************************************************\n");
        pthread_mutex_unlock(&mutex_sum);
}


void * think(int *a)
{
        int seed, s;
        init_genrand((unsigned long)&seed);
        philosophers[*a]=0;
        s = rdn_int()%20+1;
        printf("%s is %sing for %ds.\n",name[*a],stage[philosophers[*a]],s);
        sleep(s);

}

void * get_forks(int *a)
{
        do {
                int seed;
                init_genrand((unsigned long)&seed);
                int b = (*a)%5;
                int c = (*a+1)%5;
                if ((forks[b] == 0) && (forks[c] == 0)){
                        pthread_mutex_lock(&mutex_sum);
                        philosophers[*a]=1;
                        forks[b] = 1;
                        forks[c] = 1;
                        pthread_mutex_unlock(&mutex_sum);
                        printf("%s got fork %d and %d.\n",name[*a],b,c);
                }
                break;
        }
        while(1);
}

void * eat(int *a)
{
        int seed,s;
        init_genrand((unsigned long)&seed);
        s = rdn_int()%8+2;
        eat_count[*a]++;
        printf(RED "%s is %sing for %ds.\n"RESET ,name[*a],stage[philosophers[*a]],s);
        print_table();
        sleep(s);
        int i;
}

void * put_forks(int *a)
{
        int b = (*a)%5;
        int c = (*a+1)%5;
        pthread_mutex_lock(&mutex_sum);
        forks[b]=0;
        forks[c]=0;
        philosophers[*a]=0;
        printf(GREEN"%s put down fork %d and %d.\n"RESET, name[*a], b, c);
        pthread_mutex_unlock(&mutex_sum);
}

void * dinersProblem(int *a)
{
        do {
                think(a);
                get_forks(a);
                if (philosophers[*a] == 1){
                        eat(a);
                        put_forks(a);
                        print_table();
			}
        }
        while(1);

}

int main()
{
        pthread_t *threads;
        int v1 = 1;
        int v2 = 2;
        int v3 = 3;
        int v4 = 4;
        int v5 = 5;

        threads = (pthread_t *) malloc(5 * sizeof(pthread_t));
        pthread_mutex_init(&mutex_sum, NULL);

        pthread_create(&threads[0], NULL, dinersProblem,(int *) &v1);
        pthread_create(&threads[1], NULL, dinersProblem,(int *) &v2);
        pthread_create(&threads[2], NULL, dinersProblem,(int *) &v3);
        pthread_create(&threads[3], NULL, dinersProblem,(int *) &v4);
        pthread_create(&threads[4], NULL, dinersProblem,(int *) &v5);

        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        pthread_join(threads[2], NULL);
        pthread_join(threads[3], NULL);
        pthread_join(threads[4], NULL);
}

