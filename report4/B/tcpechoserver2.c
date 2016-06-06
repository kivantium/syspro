#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    char buffer[1025];
    fd_set readfds;

    if(argc != 2) {
        fprintf(stderr, "Usage %s (port number)\n", argv[0]);
        exit(-1);
    }

    for (i=0; i<max_clients; i++) {
        client_socket[i] = 0;
    }

    // master socketの設定
    if( (master_socket = socket(AF_INET, SOCK_STREAM , 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        for (i=0 ; i<max_clients; i++) {
            sd = client_socket[i];
            if(sd > 0) FD_SET(sd, &readfds);
            if(sd > max_sd) max_sd = sd;
        }

        if (select(max_sd+1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // master sockerに何か起きたとき
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            // 適当なsocketに割り当て
            for (i=0; i<max_clients; i++) {
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    break;
                }
            }
        }

        for (i=0; i<max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd , buffer, 1024)) == 0) {
                    // 切断された場合
                    close(sd);
                    client_socket[i] = 0;

                } else { 
                    // echo 
                    buffer[valread] = '\0';
                    send(sd, buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
    return 0;
} 
