/* *******************************************
 * Program  : concurrency practice 5 problem 2
 * Objective: barbershop problem
 * Author   : Chauncey Yan, Xilun Guo, Xiaomei Wang 
 * Date     : Nov 15th
 * ******************************************/
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include<errno.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"
#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

int customers = 0;      // number of customers
int seats = 4;          // number of chairs
int cut_time;           // duration for the haircut
int duration = 8;       // duration uplimit for the haircut
int period = 4;         // period uplimit for customer spwan
int sleeping = 1, iseed;// others
sem_t sem_s;            // for barber seat
sem_t sem_c;            // for customer
sem_t sem_b;            // for barber

void get_hair_cut();
void cut_hair();
void customer_create();
void customer();
void barber();

/* *******************************************
 * Main
 * ******************************************/
int main(){
        iseed=time(NULL);
        srand(iseed);

        sem_init(&sem_s,0,1);    // pshared set to 0; initialize sem to 1
        sem_init(&sem_c,0,0);
        sem_init(&sem_b,0,0);

        pthread_t thread[2]; 
        pthread_create(&thread[0], NULL, (void *) &barber, NULL);
        pthread_create(&thread[1], NULL, (void *) &customer_create, NULL);
        pthread_join(thread[0],NULL);
        pthread_join(thread[1],NULL);

        sem_destroy(&sem_s); /* destroy semaphore */
        sem_destroy(&sem_c); /* destroy semaphore */
        sem_destroy(&sem_b); /* destroy semaphore */

        /* exit */  
        exit(0);
}



/* *******************************************
 * barber cut hair
 * detemine duration and sleep
 * ******************************************/
void cut_hair(){    
        sleep(cut_time);
        printf(GREEN"CutHair done and barber idle.\n"RESET);
}

/* ****************************************** 
 * customer get hair cut
 * customers - 1 after sleep duration
 * ******************************************/
void get_hair_cut(){    
        sleep(cut_time);
        printf(GREEN"GetHaircut done and customer left.\t\t\t\t\t%d Customers in store.\n"RESET,--customers);
}
/* ****************************************** 
 * new customers comes in every 1-3 seconds
 * ******************************************/
void customer_create(){ 
        int i = 0,s;
        pthread_t cus_t[seats+1]; 
        while(1){
                sleep(rand()%(period-1)+2);
                customers++;
                s = pthread_create(&cus_t[i], NULL, (void *) &customer, NULL);
                i++;
                if (i == seats)
                        i = 0;
                if (s != 0)
                        handle_error_en(s, "pthread_create");
        }

}
/* ****************************************** 
 * customer check seats and wait
 * ******************************************/
void customer(){
        printf(BLUE"\nNew customer has arrived in the waiting room.\n"RESET,customers);
        if(customers <= seats){
                if(sleeping == 1){
                        printf(RED"Barber is sleeping, customer wakes him.\n"RESET);
                        sleeping = 0;
                        sem_post(&sem_b);       // wake up the barber thread
                }
                printf(BLUE"New customer takes a seat.\t\t\t\t\t\t%d Customers in store.\n\n"RESET,customers);
                sem_wait(&sem_c);               // lock the customer util barber wake this customer
                get_hair_cut();
                sem_post(&sem_s);               // unlock barber seat for other customer

        }else if (customers > seats){
                customers--;
                printf(RED"All %d seats are full, leaving store.\t\t\t\t\t%d Customers in store.\n\n"RESET,seats,customers);
        }
        pthread_exit(0);
}

/* ****************************************** 
 * barber sleep and give haircut
 * ******************************************/
void barber(){
        printf("The barber has opened the store.\n %d seats available.\n",seats);
        printf("The barber haircut duration is set to 2-%d seconds\n",duration);
        printf("The customer spawn period is set to 2-%d seconds\n",period);
        while(1){
                sem_wait(&sem_s);               // lock the barber seat for this customer
                if( customers == 0 ){
                        sleeping = 1;
                        printf(RED"\nThe shop is empty, barber is sleeping.\n"RESET);
                        sem_wait(&sem_b);       // lock barber
                        sem_post(&sem_s);       // unlock barber seat for nobody
                }
                else {
                        cut_time = rand()%(duration-1)+2;   // haircut duration 
                        sem_post(&sem_c);       // wake up one customer
                        printf(YELLOW"\nBarber begins to cut hair.\n"RESET);
                        printf(YELLOW"Haircut will take %dsec\n"RESET,cut_time);
                        cut_hair();
                }
        }
}
