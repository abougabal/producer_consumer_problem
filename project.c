/* buffer.h */
typedef int buffer_item;
#define BUFFER_SIZE 5

/* main.c */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define RAND_DIVISOR 100000000 // just a random number 
#define TRUE 1
/* The mutex lock */
pthread_mutex_t mutex;

/* the semaphores */
sem_t full, empty;

/* the buffer */
buffer_item buffer[BUFFER_SIZE]; // the shared buffer 

/* buffer counter */
int counter; // counter to keep track of the buffer size currently

pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void* producer(void* param); /* the producer thread */
void* consumer(void* param); /* the consumer thread */
buffer_item buffer[BUFFER_SIZE];
int insert_item(buffer_item item) // insert an item if their is a free space in the buffer
{ 
	/* 
	insert item into buffer return 0 if successful,
		otherwise return -1 indicating an error condition 
		*/ 
	if (counter < BUFFER_SIZE) // checking that their is an empty slot in the buffer 
	{
		buffer[counter] = item;
		counter++;
		return 0;
	}
	else // no empty space
		return -1;
}
int remove_item(buffer_item* item)  //removing an item if their is an availble item to be consummed in the buffer 
{ 
	/*
	 remove an object from buffer placing it in item return 0 if successful,
	 otherwise return -1 indicating an error condition 
	*/ 
	if (counter > 0) //checking that the buffer have atleast 1 item in it
	{
		*item = buffer[(counter-1)];
		counter--;
		return 0;
	}
	else // no products to consume
		return -1;
}
void* producer(void* param) //producer thread 
{ 
	buffer_item item;
	while (TRUE)
	{
		int ranval = rand() / RAND_DIVISOR;

		/* sleep for a random period of time */
		sleep(ranval);
		/* generate a random number */
		item = rand();
		sem_wait(&empty); // acquire empty lock
		pthread_mutex_lock(&mutex); // acquire mutex lock
		if (insert_item(item))
			fprintf(stderr, "producer report error condition\n");
		else
			printf("producer produced %d\n", item);
		pthread_mutex_unlock(&mutex); // release the mutex lock
		sem_post(&full); //signal full
	}
}
	
	void* consumer(void* param) // consumer thread 
	{
		buffer_item item;

		while (TRUE) {
			/* sleep for a random period of time */
			int rNum = rand() / RAND_DIVISOR;
			sleep(rNum);

			/* aquire the full lock */
			sem_wait(&full);
			/* aquire the mutex lock */
			pthread_mutex_lock(&mutex);
			if (remove_item(&item)) {
				fprintf(stderr, "Consumer report error condition\n");
			}
			else {
				printf("consumer consumed %d\n", item);
			}
			/* release the mutex lock */
			pthread_mutex_unlock(&mutex);
			/* signal empty */
			sem_post(&empty);
		}
	}


int main(int argc, char* argv[])
{ 
	int i; // counter value
	counter = 0;
	/* 1. Get command line arguments argv[1],argv[2],argv[3] */
	if (argc != 4)
	{
		fprintf(stderr, "not enough arguments \n");
	}
	int mainSleepTime = atoi(argv[1]); /* Time in seconds for main to sleep */
	int numProd = atoi(argv[2]); /* Number of producer threads */
	int numCons = atoi(argv[3]); /* Number of consumer threads */

	

	/* 2. Initialize buffer */
	pthread_mutex_init(&mutex, NULL); // initalizing mutex 
	sem_init(&full, 0, 0); // non shared semaphore with initial value = 0
	sem_init(&empty, 0, BUFFER_SIZE); // non shared semaphore with initial value = buffer_size
	pthread_attr_init(&attr); // get the standard attr
	/* 3. Create producer thread(s) */ 
	for (i = 0; i < numProd; i++)
	{
		pthread_create(&tid, &attr, producer, NULL); // creating producer threads
	}
	/* 4. Create consumer thread(s) */
	for (i = 0; i < numCons; i++)
	{
		pthread_create(&tid, &attr, consumer, NULL); // creating consumer threads
	}
	/* 5. Sleep */
	sleep(mainSleepTime); // sleeping for the time that the user put in args[1]
	/* 6. Exit */
	printf("the time of the program is finished \n");
	exit(0);
}