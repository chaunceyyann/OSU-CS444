#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "mt19937ar.h"

#define NUM_OF_SEATS 6
#define NUM_OF_CUSTOMERS 20

int available_wait_seats = NUM_OF_SEATS;

sem_t barberSem;
sem_t customerSem;
pthread_mutex_t wait_seats;

void *get_hair_cut(void* arg){ 
     int id = *((int *)arg);
     while(1){
     	pthread_mutex_lock(&wait_seats);
     	if(available_wait_seats > 0){
     		usleep(500000);
     		printf("customer %d is entering barbershop\n", id);
     		available_wait_seats--;
     		sem_post(&customerSem);     		
     		pthread_mutex_unlock(&wait_seats);
     		usleep(200000);
     		sem_wait(&barberSem);
     		printf("customer %d's hair\n", id);
     		usleep(100000);
     		printf("customer %d haircut is finished and leaving now.\n", id);
     		usleep(900000);
     	}
     	else{
     		pthread_mutex_unlock(&wait_seats);
     		printf("customer %d leaving because waiting room is full\n", id);
     		usleep(900000);
     	}
     }
} 

void *cut_hair(void* arg){
	while(1){
		if(available_wait_seats <= NUM_OF_SEATS){
			//usleep(400000);
			sem_wait(&customerSem);
			pthread_mutex_lock(&wait_seats);
			available_wait_seats++;
			sem_post(&barberSem);
			printf("barber is cutting --\n");
			pthread_mutex_unlock(&wait_seats);
			//printf("barber is cutting --\n");
			usleep(500000);
		}
		else{
			printf("barber is sleeping\n");
		}
	}
}

int main(){
	int ids[NUM_OF_CUSTOMERS];
	pthread_t barberThread;
	pthread_t customerThreads[NUM_OF_CUSTOMERS];

	pthread_mutex_init(&wait_seats, NULL);
	sem_init(&barberSem, 0, 0);
	sem_init(&customerSem, 0, 0);

	pthread_create(&barberThread, NULL, cut_hair, NULL);
	
	int i;
	for(i = 0; i < NUM_OF_CUSTOMERS; i++){
		ids[i] = i;
		pthread_create(&customerThreads[i], NULL, get_hair_cut, &ids[i]);
	}
	for(i = 0; i < NUM_OF_CUSTOMERS; i++){
		pthread_join(customerThreads[i], NULL);
	}

	return 0;
}
