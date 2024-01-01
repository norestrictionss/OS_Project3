#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread/pthread.h>

typedef struct {
    long long int start;
    long long int end;
} ThreadArgs;

double global_sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *method_1(void *threadArgs) {
    ThreadArgs *args = (ThreadArgs *) threadArgs;
    long long start = args->start;
    long long end = args->end;
    for (long long int i = start; i <= end; ++i) {
        global_sum += sqrt(i);
    }
    free(args);
    return NULL;
}

void *method_2(void *threadArgs) {
    ThreadArgs *args = (ThreadArgs *) threadArgs;
    long long start = args->start;
    long long end = args->end;
    pthread_mutex_lock(&mutex);
    for (long long int i = start; i <= end; ++i) {
        global_sum += sqrt(i);
    }
    pthread_mutex_unlock(&mutex);

    free(args);
    return NULL;

}

void *method_3(void *threadArgs) {
    ThreadArgs *args = (ThreadArgs *) threadArgs;
    long long int start = args->start;
    long long int end = args->end;
    double local_sum = 0;

    for (long long int i = start; i <= end; ++i) {
        local_sum += sqrt(i);
    }

    pthread_mutex_lock(&mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);

    free(args); // Free memory for ThreadArgs
    return NULL;
}


int main(int argc, char *argv[]) {
    long long int a; // start of range
    long long int b; // end of range
    int c;    // thread number
    int d;    // method number

    if (argc != 5) {
        printf("Usage: %s <a> <b> <c> <d>\n", argv[0]);
        return -1;
    }
    // input variables

    a = atoll(argv[1]);
    b = atoll(argv[2]);
    c = atoi(argv[3]);
    d = atoi(argv[4]);

    // Check if the conversion was successful
    if (a == LLONG_MAX || a == LLONG_MIN) {
        printf("Invalid input for a\n");
        return EXIT_FAILURE;
    }
    if (b == LLONG_MAX || b == LLONG_MIN) {
        printf("Invalid input for b\n");
        return EXIT_FAILURE;
    }
    if (a > b) {
        printf("Invalid range for [a,b]\n");
        return EXIT_FAILURE;
    }
    // Check if c  are within acceptable ranges
    if (c <= 0) {
        printf("Invalid input for c");
        return EXIT_FAILURE;
    }
    // Check if d are within acceptable ranges
    if (d < 1 || d > 3) {
        printf("invalid input for d\n");
        return EXIT_FAILURE;
    }
    // initialize thread array
    pthread_t threads[c];
    // every thread have a start and end
    long long int currentStart;
    long long int currentEnd;
    long long int n;
    for (int i = 0; i < c; ++i) {
        n = (b - a + 1) / c;
        currentStart = a + (i * n);
        currentEnd = currentStart + n - 1;
        ThreadArgs *args = (ThreadArgs *) malloc(sizeof(ThreadArgs));
        args->start = currentStart;
        if (i == c - 1) {
            args->end = b;

        } else {
            args->end = currentEnd;
        }
        int thread_create_status = pthread_create(&threads[i], NULL,
                                                  (d == 1) ? method_1 : ((d == 2) ? method_2 : method_3),
                                                  (void *) args);
        if (thread_create_status != 0) {
            fprintf(stderr, "Error creating thread %d. Exiting...\n", i);
            // Handle error or cleanup resources if needed
            exit(EXIT_FAILURE);
        }
    }
    // Wait for all threads to finish
    for (int i = 0; i < c; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("Sum: %f\n", global_sum);
    return 0;
}

