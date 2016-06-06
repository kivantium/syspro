#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv, char *envp[]) {
    pid_t pid;
    int status;
    if( (pid = fork()) == 0) {
       execve(argv[1], &argv[1], envp);
    } else {
        waitpid( pid, &status, WUNTRACED );
    }

    return 0;
}
