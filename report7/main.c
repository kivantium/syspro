#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"

void sigint_handler(int sig) {
    ;
}
void sigchld_handler(int sig) {
    wait(NULL);
}
int main(int argc, char *argv[], char *envp[]) {
    char s[LINELEN];
    job *curr_job;
    job*     jb;
    process* pr;
    pid_t pid;
    struct sigaction si;
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, sigchld_handler);

    sigset_t sigset;
    sigemptyset(&sigset);

    si.sa_sigaction = NULL;
    si.sa_handler = sigint_handler;
    si.sa_mask = sigset;
    si.sa_flags = 0;

    sigaction(SIGINT, &si, NULL);

    while(get_line(s, LINELEN)) {
        if(!strcmp(s, "exit\n")) break;
        curr_job = parse_line(s);

        for(jb = curr_job; jb != NULL; jb = jb->next) {
            int n = 0;
            int old_fd[2], new_fd[2];
            pid_t pid, pgid;
            pr = jb->process_list;
            for(;pr != NULL; pr=pr->next) n++;
            pr = jb->process_list;
            for(int i=0; pr != NULL; pr=pr->next, ++i) {
                if(pr->next != NULL) {
                    pipe(new_fd);
                }

                if((pid = fork()) < 0) {
                    perror("error in fork");
                    exit(EXIT_FAILURE);
                } else if(pid == 0) {
                    if(i > 0) {
                        dup2(old_fd[0], 0);
                        close(old_fd[0]);
                        close(old_fd[1]);
                    }
                    if(pr->next != NULL) {
                        close(new_fd[0]);
                        dup2(new_fd[1], 1);
                        close(new_fd[1]);
                    }
                    if(i==0 && pr->input_redirection != NULL) {
                        int fd;
                        if((fd = open(pr->input_redirection, O_RDONLY,
                                        0666)) < 0) {
                            perror("error opening input file");
                            return 0;
                        }
                        dup2(fd, 0);
                    }
                    if(pr->next == NULL && pr->output_redirection != NULL){
                        int fd;
                        if(pr->output_option == TRUNC) 
                            fd = open(pr->output_redirection, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        else
                            fd = open(pr->output_redirection, O_CREAT | O_WRONLY | O_APPEND, 0644);
                        dup2(fd, 1);
                    }
                    if(i==0) {
                        pgid = getpid();
                        setpgid(0, 0);
                    } else {
                        setpgid(0, pgid);
                    }
                    if(execve(pr->argument_list[0], pr->argument_list, envp) < 0) {
                        perror("error executing command");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    if(i==0) {
                        if(tcsetpgrp(0, pid) < 0) perror("tcsetpgrp");
                    }
                    if(i > 0) {
                        close(old_fd[0]);
                        close(old_fd[1]);
                    }
                    if(pr->next != NULL) {
                        old_fd[0] = new_fd[0];
                        old_fd[1] = new_fd[1];
                    }
                }
            }
            if (n > 1) {
                close(old_fd[0]);
                close(old_fd[1]);
            }
            if(jb->mode == FOREGROUND) for(int i=0; i<n; ++i) wait(NULL);
            if(jb->next != NULL)  printf("\n");
        }
        tcsetpgrp(0, getpgid(0));
        free_job(curr_job);
    }

    return 0;
}
