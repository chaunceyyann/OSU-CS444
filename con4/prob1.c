/* *******************************************
 * Program  : concurrency practice 4 problem 1
 * Objective: As long as there are fewer than 
 * three processes using the resource, new 
 * processes can start using it right away.
 * Once there are three processes using the 
 * resource, all three must leave before any 
 * new processes can begin using it.
 * Author   : Chauncey Yan, Xilun Guo, Xiaomei Wang
 * Date     : Nov 17th
 * ******************************************/
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>

int i, wait_cnt;            // count down how many process
pthread_t thread[20];       // 20 processes
pthread_t pth;              // check if 3 or more used resource
sem_t sem0;                 // muturely exclusive for processes and checker
sem_t sem1;                 // act as a counter
int num=0;                  // shared resource

void dosome();
void checker();
int main(){
    sem_init(&sem0,0,1);    // pshared set to 0; initialize sem to 1
    sem_init(&sem1,0,3);
    pthread_create(&pth, NULL, (void *) &checker, NULL);
    for (i = 0; i<20; i++){
        usleep(200000);
        pthread_create(&thread[i], NULL, (void *) &dosome, NULL);
        pthread_join(thread[i], NULL);
    }
    //pthread_join(pth, NULL);
    return 0;
}


/* *******************************************
 * dosome
 * ******************************************/
void dosome(){
    num+=1;
    sem_wait(&sem0);
    sem_wait(&sem1);
    printf("Process ID: %d.\n",num);
    sem_post(&sem0);
    pthread_exit(0);
}
/* *******************************************
 * checker
 * ******************************************/
void checker(){
    while(1){
        sem_getvalue(&sem1,&wait_cnt);
        if (wait_cnt <= 0){
            sem_wait(&sem0);
            printf(" 3 or more process.\n");
            sem_post(&sem1);    // add 1
            sem_post(&sem1);    // add 1
            sem_post(&sem1);    // add 1
            sem_post(&sem0);
        }
    }
}

