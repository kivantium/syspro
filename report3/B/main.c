#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 10

int bb_buf[SIZE];
int head = 0, tail = 0, data = 0, count_put = 0, count_get = 0;
pthread_cond_t waitUntilData = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitUntilEmpty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int bb_get(void) {
    int ret;
    pthread_mutex_lock(&mutex);
    while(data <= 0) {
        pthread_cond_wait(&waitUntilData, &mutex);
    }
    ret = bb_buf[head];
    data--;
    head++;
    count_get++;
    if(head == SIZE) head = 0;
    fprintf(stderr, "%d : %d\n", count_get, ret);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&waitUntilEmpty);
    return ret;
}

void bb_put(int i) {
    pthread_mutex_lock(&mutex);
    while(data >= SIZE) {
        pthread_cond_wait(&waitUntilEmpty, &mutex);
    }
    bb_buf[tail] = i;
    data++;
    tail++;
    count_put++;
    if(tail == SIZE) tail = 0;
    printf("%d : %d\n", count_put, i);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&waitUntilData);
}

void *put(void *arg) {
    int i;
    unsigned int seed = 100;
    for(i=0; i<1000; ++i) {
        bb_put(rand_r(&seed));
    }
    pthread_exit(NULL);
}

void *get(void *arg) {
    int i;
    for(i=0; i<1000; ++i) {
        bb_get();
    }
    pthread_exit(NULL);
}

int main() {
    int i;
    pthread_t thread[100];
    for(i=0; i<100; ++i) {
        if(pthread_create(&thread[i], NULL, put, NULL) != 0) {
            fprintf(stderr, "error: pthread_create\n");
            return -1;
        }
        if(pthread_create(&thread[i], NULL, get, NULL) != 0) {
            fprintf(stderr, "error: pthread_create\n");
            return -1;
        }
    }

    for(i=0; i<100; ++i) {
        if(pthread_join(thread[i], NULL) != 0) {
            fprintf(stderr, "error: pthread_join\n");
            return 1;
        }
    }
}
