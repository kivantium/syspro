#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#define MAXCONNECT 10
#define NTHREAD 10
#define BUFFSIZE 256

void err_exit(char msg[]) {
    perror(msg);
    exit(-1);
}
void *server(void *arg) {
    int s, ss = *(int *)arg;
    struct sockaddr_in echo_client;
    unsigned int client_len = 0;
    char buffer[BUFFSIZE];
    int rec_size;

    while(1) {
        client_len = sizeof(echo_client);
        s = accept(ss, (struct sockaddr *) &echo_client, &client_len);
        if(s < 0) {
            err_exit("error in accept()");
        }

        // recv() でクライアントからのデータ読み込み
        rec_size = recv(s, buffer, BUFFSIZE, 0);
        if(rec_size < 0) {
            err_exit("error in recv()");
        } else {
            buffer[rec_size] = 0;
        }

        // エコー処理を繰り返す
        while(rec_size > 0){
            if(send(s, buffer, rec_size, 0) != rec_size) {
                err_exit("error in send()");
            }
            if((rec_size = recv(s, buffer, BUFFSIZE, 0)) < 0) {
                err_exit("error in recv()");
            }
        }
        close(s);
    }
}

int main(int argc, char *argv[]) {
    int sock_server;
    struct sockaddr_in echo_server;
    int i;
    pthread_t thread[NTHREAD];

    if(argc != 2) {
        fprintf(stderr, "Usage %s (port number)\n", argv[0]);
        exit(-1);
    }

    if((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0) {
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

    for(i=0; i<NTHREAD; ++i) {
        if(pthread_create(&thread[i], NULL, server, &sock_server) != 0) {
            err_exit("error in pthread_create()");
        }
    }
    for(i=0; i<NTHREAD; ++i) {
        if(pthread_join(thread[i], NULL) != 0) {
            err_exit("error in pthread_join()");
        }
    }
}
