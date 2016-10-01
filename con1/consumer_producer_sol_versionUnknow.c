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

extern void *consumer_problem(void *a);
extern void *producer_problem(void *a);

sem_t *s1;
sem_t *s2;

struct t_elements {
        int num;
        int sec;

};

struct t_elements buffer[32];

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

void *consumer_problem(void *a)
{
        do {
                int i = 0;
                int z = 0;
                if (buffer[0].num == NULL) {
                        sem_wait(s2);
                }
                pthread_mutex_lock(&mutex_sum);
                sleep(buffer[0].sec);
                printf("The number is: %d\n", buffer[0].num);
                fflush(stdout);

                for (z = 31; z >= 0; z--) {
                        if (buffer[z].num != NULL) {
                                memcpy(buffer, &buffer[1], 
                                               z*sizeof(struct t_elements));
                                buffer[z].num = NULL;
                                buffer[z].sec = NULL;
                                sem_post(s1);
                                break;
                        }
                }

                pthread_mutex_unlock(&mutex_sum);
        }
        while (1);
}
void *producer_problem(void *a)
{
        do {
                if (buffer[31].num != NULL) {
                        sem_wait(s1);
                }
                int seed;
                init_genrand((unsigned long)&seed);
                int j = 0;
                sleep(rnd_int()%5+3);
                pthread_mutex_lock(&mutex_sum);

                for (j = 0; j < 32; j++) {
                        if (buffer[j].num == NULL) {
                                unsigned long init[4] = {0x123, 0x234,
                                                       0x345, 0x456}, length = 4;

                                buffer[j].num = rnd_int() % 8 + 2;
                                fflush(stdout);
                                buffer[j].sec = rnd_int() % 8 + 2;
                                fflush(stdout);
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
        char *c = "semaphore 1";
        char *b = "semaphore 2";

        s1 = sem_open(c, O_CREAT, 0666, 0);
        s2 = sem_open(b, O_CREAT, 0666, 0);
        int num1 = atoi(argv[1]);
        int num2 = atoi(argv[2]);
        int s = 0;
        int z = 0;
        pthread_t *threads;

        threads = (pthread_t *) malloc((num1 + num2) * sizeof(pthread_t));
        pthread_mutex_init(&mutex_sum, NULL);
        for (s = 0, z = 0; (s + z) < (num1 + num2);) {
                if (s < num1) {
                        pthread_create(&threads[s], NULL, 
                                       producer_problem,NULL);
                        s++;
                }
                if (z < num2 ) {
                        pthread_create(&threads[num1 + z], NULL,
                                       consumer_problem,NULL);
                        z++;
                }
        }
        for (s = 0; s < (num1 + num2); s++) {
                pthread_join(threads[s], NULL);
        }
}

