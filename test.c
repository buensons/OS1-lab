#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MS 100
#define ERROR(source) (perror(source), fprintf(stderr, "%s:%d", __FILE__, __LINE__), exit(EXIT_FAILURE))

typedef struct threadargs {
    float * array;
    float * result_array;
    int arrsize;
    int * remaining;
    unsigned int seed;
    pthread_mutex_t * mutex;
} threadargs_t;

void usage(char * p) {
    fprintf(stderr, "usage: %s n k\n", p);
    exit(EXIT_FAILURE);
}

// thread working function
void * thread_fun(void * arg) {
    struct timespec t = {0, 1000000 * MS};
    threadargs_t * args = arg;
    int index;
    while(*args->remaining > 0) {
        index = rand_r(&args->seed) % (args->arrsize);
        pthread_mutex_lock(&args->mutex[index]);
        if(args->result_array[index] < 1) {
            args->result_array[index] = sqrt(args->array[index]);
            *args->remaining -= 1;
            printf("%d\n", *args->remaining);
        }
        pthread_mutex_unlock(&args->mutex[index]);
        nanosleep(&t, NULL);
    }
    pthread_exit(0);
}

// -------- MAIN --------

int main(int argc, char ** argv) {
    // args
    if(argc != 3) usage(argv[0]);
    int n = atoi(argv[1]);
    int k = atoi(argv[2]);

    //arrays 
    float task_arr[k];
    float result_array[k];
    memset(result_array, 0, sizeof(result_array));

    threadargs_t args[n+1];
    pthread_t tid[n+1];
    pthread_mutex_t mutex[k];
    int remaining_cells = k;
    srand(time(NULL));

    // fill array with random floats
    for(int i = 0; i < k; i++) {
        task_arr[i] = (float)(rand() % 60 + 1);
    }

    // dynamic mutex initialization
    for(int i = 0; i < k; i++) {
        if(pthread_mutex_init(&mutex[i], NULL)) ERROR("mutex_init");
    }

    // threads creation
    for(int i = 0; i <= n; i++) {
        args[i].mutex = mutex;
        args[i].seed = rand();
        args[i].remaining = &remaining_cells;
        args[i].array = task_arr;
        args[i].result_array = result_array;
        args[i].arrsize = sizeof(task_arr) / sizeof(task_arr[0]);
        if(pthread_create(&tid[i], NULL, thread_fun, &args[i])) ERROR("pthread_create");
    }

    // wait for threads
    for(int i = 0; i <= n; i++) {
        if(pthread_join(tid[i], NULL)) ERROR("pthread_join");
    }

    // destroy dynamic mutexes
    for(int i = 0; i < k; i++) {
        if(pthread_mutex_destroy(&mutex[i])) ERROR("mutex_destroy");
    }

    // print results
    printf("task array\n");
    for(int i = 0; i < k; i++) {
        printf("%fd ", task_arr[i]);
    }
    printf("\nresult array\n");
    for(int i = 0; i < k; i++) {
        printf("%fd ", result_array[i]);
    }
    printf("\n");
    return EXIT_SUCCESS;
}

