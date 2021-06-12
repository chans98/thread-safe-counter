#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

#define PATH "/home/Users"

unsigned int loop_cnt;
counter_t counter;

void init(counter_t *c) {
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}

void *mythread(void *arg)
{
    char *letter = arg;
    int i;

    printf("%s: begin\n", letter);
    for (i = 0; i < loop_cnt; i++) {
        increment(&counter);
    }
    printf("%s: done\n", letter);
    return NULL;
}
                                                                             
int main(int argc, char *argv[])
{                    
    loop_cnt = atoi(argv[1]);

    init(&counter);

    pthread_t p1, p2;
    printf("main: begin [counter = %d]\n", get(&counter));
    pthread_create(&p1, NULL, mythread, "A"); 
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    printf("main: done [counter: %d] [should be: %d]\n", get(&counter), loop_cnt * 2);
    return 0;
}



//semaphore

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <semaphore.h>

void * read( void * arg );
void * accu( void * arg );

static sem_t sem_one;
static sem_t sem_two;
static int num;

int main( int argc, char *argv[] )
{
	pthread_t id_t1, id_t2;
    
	sem_init( &sem_one, 0, 0 );
	sem_init( &sem_two, 0, 1 );

	pthread_create( &id_t1, NULL, read, NULL );
	pthread_create( &id_t2, NULL, accu, NULL );

	pthread_join( id_t1, NULL );
	pthread_join( id_t2, NULL );
	
	sem_destroy( &sem_one );
	sem_destroy( &sem_two );

	return 0;
}

void * read( void * arg )
{
	int i;

	for( i = 0 ; i < 5 ; i++ )
	{
		fputs("Input num : ", stdout );

		sem_wait( &sem_two );
		scanf("%d", &num );
		sem_post( &sem_one );
	}


}
void * accu( void * arg )
{
	int sum = 0, i;

	for( i = 0 ; i < 5 ; i++ )
	{
		sem_wait( &sem_one );
		sum += num;
		sem_post( &sem_two );
	}
	
	printf("Result : %d \n", sum );
	return NULL;
}
