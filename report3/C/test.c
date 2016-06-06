#include <stdio.h>
#include <pthread.h>

void *func (void *arg) {
    int *p = (int *)arg;
    printf("thread : arg = %d\n", *p);
    //printf("thread : arg = %d, pid: %u, parent: %u\n", *p, getpid(), getppid());
    *p += 100;
    pthread_exit(NULL);
}

int main(void) {
    pthread_t thread;
    int i = 1;
    if (pthread_create(&thread, NULL, func, &i) != 0) {
        printf("error: pthread_create\n");
        return 1;
    }
    if (pthread_join(thread, NULL) != 0) {
        printf("error: pthread_join\n");
        return 1;
    }
    printf("main : i = %d\n", i);
    return 0;
}
