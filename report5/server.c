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
#define BUFFSIZE 4096

char root[1024];

void err_exit(char msg[]) {
    perror(msg);
    exit(-1);
}

void *func(void *arg) {
    int s, ss = *(int *)arg;
    struct sockaddr_in client;
    unsigned int client_len = 0;
    char buf[BUFFSIZE], read[BUFFSIZE], method[BUFFSIZE], uri[BUFFSIZE], ver[BUFFSIZE], document[BUFFSIZE];
    char *RequestLine;
    int rec_size;
    FILE* fp;
    char *common_header = "Connection: close\r\n\r\n";

    client_len = sizeof(client);

    while(1) {
        s = accept(ss, (struct sockaddr *) &client, &client_len);
        if(s < 0) {
            err_exit("error in accept()");
        }
        int total = 0;
        memset(buf, '\0', BUFFSIZE);
        while(strstr(buf, "\r\n\r\n") == NULL) {
            rec_size = recv(s, buf+total, BUFFSIZE-total, 0);
            // error in receive
            if(rec_size < 0) {
                err_exit("error in recv()");
            }
            total += rec_size;
            // too long request
            if(total == BUFFSIZE) {
                char *m = "HTTP/1.1 400 Bad Request\n";
                send(s, m, strlen(m), 0);
                close(s);
                goto ERROR;
            }
        }
        printf("%s", buf);
        RequestLine = strtok(buf, "\n\n");
        sscanf(RequestLine, "%s %s %s", method, uri, ver);

        // check version
        if(strncmp(ver, "HTTP/1.1\0", 9)!=0 && strncmp(ver, "HTTP/1.0\0", 9)!=0) {
            char *m = "HTTP/1.1 400 Bad Request\n";
            send(s, m, strlen(m), 0);
            goto ERROR;
        }

        if (strcmp(method, "GET")==0) {
            // convert / into /index.html
            if(strncmp(uri, "/\0", 2) == 0) {
                strcpy(uri, "/index.html");
            }
            // avoid opening parent folder
            if(strstr(uri, "../") != NULL) {
                char *header = "HTTP/1.1 403 Forbidden\r\n\r\n";
                send(s, header, strlen(header), 0);
                goto ERROR;
            }
            strcpy(document, root);
            strcat(document, uri);
            // printf("open document: %s\n", document);
            if ((fp = fopen(document, "r")) == NULL) {
                fprintf(stderr, "Failed to open %s\n", document);
                char *header = "HTTP/1.1 404 Not Found\n\n";
                send(s, header, strlen(header), 0);
            } else {
                char *header = "HTTP/1.1 200 OK\n";
                send(s, header, strlen(header), 0);
                send(s, common_header, strlen(common_header), 0);
                while(fgets(read, sizeof(read), fp) != NULL) {
                    send(s, read, strlen(read), 0);
                }
                fclose(fp);
            }
        }else if(strcmp(method, "HEAD")==0) {
            char *header = "HTTP/1.1 200 OK\n";
            send(s, header, strlen(header), 0);
            send(s, common_header, strlen(common_header), 0);
        } else {
            char *m = "HTTP/1.1 402 Payment Required\n";
            send(s, m, strlen(m), 0);
        }
ERROR:
        close(s);
    }
}

int main(int argc, char *argv[]) {
    int sock_server;
    struct sockaddr_in server;
    int i;
    pthread_t thread[NTHREAD];

    if(argc != 3) {
        fprintf(stderr, "Usage %s (port) (document root)\n", argv[0]);
        exit(-1);
    }
    strcpy(root, argv[2]);
    printf("%s\n", argv[2]);

    if((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        err_exit("error in socket()");
    }

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[1]));

    if(bind(sock_server, (struct sockaddr *)&server,
                sizeof(server)) < 0) {
        err_exit("error in bind()");
    }

    if(listen(sock_server, MAXCONNECT) < 0) {
        err_exit("error in listen()");
    }

    for(i=0; i<NTHREAD; ++i) {
        if(pthread_create(&thread[i], NULL, func, &sock_server) != 0) {
            err_exit("error in pthread_create()");
        }
    }
    for(i=0; i<NTHREAD; ++i) {
        if(pthread_join(thread[i], NULL) != 0) {
            err_exit("error in pthread_join()");
        }
    }
}
