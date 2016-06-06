#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

int exec_process(process* pr, char *envp[]) {
    int status, stdin_old, stdout_old;
    pid_t pid;

    if(pr->program_name == NULL) {
        return -1;
    }

    if(pr->input_redirection != NULL) {
        int fd;
        stdin_old = dup(STDIN_FILENO);
        if((fd = open(pr->input_redirection, O_RDONLY, 0666)) < 0) {
            perror("error opening input file");
            return 0;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    if (pr->output_redirection != NULL) {
        int fd;
        stdout_old = dup(STDOUT_FILENO);
        if(pr->output_option == TRUNC) 
            fd = open(pr->output_redirection, O_CREAT | O_WRONLY, 0666);
        else
            fd = open(pr->output_redirection, O_CREAT | O_WRONLY | O_APPEND, 0666);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if(pr->argument_list != NULL) {
        if((pid = fork()) == 0) {
            if(execve(pr->argument_list[0], pr->argument_list, envp) < 0) {
                perror("error executing command");
                exit(EXIT_FAILURE);
            }
        } else {
            waitpid(pid, &status, WUNTRACED );
            dup2(stdin_old, STDIN_FILENO);
            dup2(stdout_old, STDOUT_FILENO);
        }
    }
    return 0;
}

int main(int argc, char *argv[], char *envp[]) {
    char s[LINELEN];
    job *curr_job;
    job*     jb;
    process* pr;

    while(get_line(s, LINELEN)) {
        if(!strcmp(s, "exit\n")) break;
        curr_job = parse_line(s);

        for(jb = curr_job; jb != NULL; jb = jb->next) {
            for(pr = jb->process_list; pr != NULL; pr = pr->next) {
                if(exec_process(pr, envp) < 0) exit(EXIT_FAILURE);

                if(jb->next != NULL)  printf("\n");
            }
        }
        free_job(curr_job);
    }

    return 0;
}
