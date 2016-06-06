#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int i;
    int ID;
    struct timeval start, end;

    // call getpid() once
    gettimeofday(&start, NULL);
    ID = getpid();
    gettimeofday(&end, NULL);
    printf("1 time: %ld\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec));

    // call getpid() 1000 times
    gettimeofday(&start, NULL);
    for(i=0; i<1000; ++i) {
        ID = getpid();
    }
    gettimeofday(&end, NULL);
    printf("1000 times: %ld\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec));
    return 0;
}
