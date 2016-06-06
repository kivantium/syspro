#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#define MAXCONNECT 10
#define BUFFSIZE 1024

void err_exit(char msg[]) {
    perror(msg);
    exit(-1);
}

int main(int argc, char *argv[]) {
    int sock_server, s;
    struct sockaddr_in echo_server;
    struct sockaddr_in echo_client;
    unsigned int client_len = 0;
    char buf_send[BUFFSIZE];
    int rec_size;
    int total, i;


    srand((unsigned) time(NULL));
    for(i=0; i<BUFFSIZE; ++i) {
        buf_send[i] = rand()%90 + 32;
    }

    if(argc != 2) {
        fprintf(stderr, "Usage %s (port number)\n", argv[0]);
        exit(-1);
    }

    if((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        err_exit("error in socket()");
    }

    memset(&echo_server, 0, sizeof(struct sockaddr_in));
    echo_server.sin_family = AF_INET;
    echo_server.sin_addr.s_addr = htonl(INADDR_ANY);
    echo_server.sin_port = htons(atoi(argv[1]));

    if(bind(sock_server, (struct sockaddr *)&echo_server,
                sizeof(echo_server)) < 0) {
        err_exit("error in bind()");
    }

    if(listen(sock_server, MAXCONNECT) < 0) {
        err_exit("error in listen()");
    }

    client_len = sizeof(echo_client);
    s = accept(sock_server, (struct sockaddr *) &echo_client, &client_len);
    if(s < 0) {
        err_exit("error in accept()");
    }

    total = 0;

    for(i=0; i<5000; ++i) {
        if(send(s, buf_send, BUFFSIZE, 0) != BUFFSIZE) {
            err_exit("error in send()");
        }
    }

    close(s);
    exit(0);
}
