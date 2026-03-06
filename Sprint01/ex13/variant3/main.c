#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <semaphore.h>
#include <pthread.h>


typedef struct{
	uint32_t value;			// changed to int from memory space for simplicity
	uint32_t writers_waiting;	// ammount of writers waiting
	sem_t reader_sem;		// semaphore to signal reader
	sem_t writer_sem;		// semaphore to signal writers
	pthread_mutex_t mux;		
} thread_struct;


void* reader(void* arg){
	

	thread_struct* thread_arg = (thread_struct*) arg;
	

	return NULL;
}

void* writer(void* arg){
	thread_struct* thread_arg = (thread_struct*) arg;
	
	

	return NULL;
}


int main(void){
	thread_struct thread_arg;
	pthread_mutex_init(&thread_arg.mux, NULL);
	
	sem_init(&thread_arg.writer_sem, NULL, 1);
	sem_init(&thread_arg.reader_sem, NULL, 1);
	
		
	
	sem_destroy(&thread_arg.reader_sem);
	sem_destroy(&thread_arg.writer_sem);
	
	pthread_mutex_destroy(&thread_arg.mux);
	
	return 0;
}
