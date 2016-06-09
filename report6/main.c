#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

int main(int argc, char *argv[], char *envp[]) {
    char s[LINELEN];
    job *curr_job;
    job*     jb;
    process* pr;
    pid_t pid;

    while(get_line(s, LINELEN)) {
        if(!strcmp(s, "exit\n")) break;
        curr_job = parse_line(s);

        for(jb = curr_job; jb != NULL; jb = jb->next) {
            int p[2];
            pid_t pid;
            int fd_in = 0;
            pr = jb->process_list;
            if(pr->input_redirection != NULL) {
                if((fd_in = open(pr->input_redirection, O_RDONLY, 0666)) < 0) {
                    perror("error opening input file");
                    return 0;
                }
            }
            while(pr != NULL) {
                pipe(p);
                if((pid = fork()) < 0) {
                    perror("error in fork");
                    exit(EXIT_FAILURE);
                } else if(pid == 0) {
                    dup2(fd_in, 0);
                    if(pr->next != NULL) dup2(p[1], 1);
                    else if(pr->output_redirection != NULL){
                        int fd;
                        if(pr->output_option == TRUNC) 
                            fd = open(pr->output_redirection, O_CREAT | O_WRONLY, 0666);
                        else
                            fd = open(pr->output_redirection, O_CREAT | O_WRONLY | O_APPEND, 0666);
                        dup2(fd, 1);
                        close(fd);
                    }
                    close(p[0]);
                    if(execve(pr->argument_list[0], pr->argument_list, envp) < 0) {
                        perror("error executing command");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    close(p[1]);
                    fd_in = p[0];
                    pr = pr->next;
                }
            }
            wait(NULL);
            if(jb->next != NULL)  printf("\n");
        }
        free_job(curr_job);
    }

    return 0;
}
