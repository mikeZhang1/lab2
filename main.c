#include "headers.h"


sem_t mutex;   // protect buffer


int main() {
        FILE *input  = fopen("input.txt", "r");    // input fd
	FILE *output = fopen("output.txt", "w");   // output fd

	// check the file open
	if (input == NULL || output == NULL) {
		printf("fopen failed\n");
    		exit(-1);
        }

	// initialize bounded buffer
        shared_struct *ptr = (shared_struct *) malloc(sizeof(shared_struct));
        ptr->in = 0;
        ptr->out = 0;


        // initialize semaphores
    	if (sem_init(&mutex, 0, 1) == -1) {
        	printf("Initilize semaphore failed\n");
        	exit(-1);
    	}

	// initialize pthread args, allocate space for input thread
        pthread_args *p_args = (pthread_args *) malloc(sizeof(pthread_args));
        p_args->ptr = ptr;
        p_args->fd = input;

	// initialize pthread args, allocate space for output thread
        pthread_args *c_args = (pthread_args *) malloc(sizeof(pthread_args));
        c_args->ptr = ptr;
        c_args->fd = output;

        pthread_attr_t attr;            // set of thread attributes
        pthread_attr_init(&attr);       // get the default attributes

	// setup threads for producer and consumer
	pthread_t producer_t[3];
	pthread_t consumer_t[3];

	// create 3 producer threads
	for(int i = 0; i < 3; i++){
	    	pthread_create(&producer_t[i], &attr, producer, p_args);
	}

	// create 3 consumer threads
	for(int i = 0; i < 3; i++){
	    pthread_create(&consumer_t[i], &attr, consumer, c_args);
	}

	// wait for working threads
	for(int i = 0; i < 3; i++){
        	pthread_join(consumer_t[i], NULL);
        	pthread_join(producer_t[i], NULL);
	}

        // close open files
        fclose(input);
        fclose(output);

    	// destroy semaphores
    	sem_destroy(&mutex);

        // free
        free(p_args);
        free(c_args);
        free(ptr);

        exit(0);
}
