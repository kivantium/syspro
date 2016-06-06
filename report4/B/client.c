#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFSIZE 2048

void err_exit(char err[])
{
    perror(err);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echo_server_addr;
    unsigned short echo_port;
    char *server_IP;
    char buf_send[BUFFSIZE], buf_recv[BUFFSIZE];
    char echo_buffer[BUFFSIZE];
    unsigned int echo_strlen;
    int received_bytenum, total;

    if(argc != 3) {
        fprintf(stderr, "Usage %s (host ip) (port number)\n", argv[0]);
        return -1;
    }


    // Step 1: socket() によるソケットの生成
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        err_exit("socket error");

    memset(&echo_server_addr, 0, sizeof(echo_server_addr));
    echo_server_addr.sin_family = AF_INET;
    echo_server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    echo_server_addr.sin_port = htons(atoi(argv[2]));

    // Step 1: connect() によるサーバへの接続要求
    if(connect(sock, (struct sockaddr *)&echo_server_addr, sizeof(echo_server_addr))<0)
        err_exit("connect error");


    while(fgets(buf_send, sizeof(buf_send), stdin) != NULL) {
        echo_strlen = strlen(buf_send);
        if(send(sock, buf_send, echo_strlen, 0) != echo_strlen)
            err_exit("send error");

        total = 0;
        while(total < echo_strlen){
            if((received_bytenum = recv(sock, echo_buffer, BUFFSIZE-1, 0))<=0)
                err_exit("recv error");
            total += received_bytenum;
            echo_buffer[received_bytenum] = 0;    /* end of string */
            printf("%s", echo_buffer);
        }
    }
    close(sock);
    exit(0);
}
