#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

int func (void *arg) {
    int *p = (int *) arg;
    printf("thread: arg = %d\n", *p);
    //printf("thread: arg = %d, pid: %u, parent: %u\n", *p, getpid(), getppid());
    *p += 100;
    return 0;
}

int main (void) {
    char *stack, *stackTop;
    int i = 1;

    pid_t pid;
    stack = malloc(STACK_SIZE);
    if (stack == NULL) perror("error in malloc");
    stackTop = stack + STACK_SIZE;

    pid = clone(func, stackTop, CLONE_VM|SIGCHLD, &i);
    if(pid == -1) {
        perror("clone error");
        return 1;
    }
    if(waitpid(pid, NULL, 0) == -1){
        perror("error in waitpid()");
        return 1;
    }
    printf("main : i = %d\n", i);
    return 0;
}
