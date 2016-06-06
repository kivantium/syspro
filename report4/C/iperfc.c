#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

#define BUFFSIZE 1024

void err_exit(char err[]) {
    perror(err);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in echo_server_addr;
    char buf[BUFFSIZE+1];
    unsigned int echo_strlen;
    int total, i, recv_size;
    struct timeval start, end;

    if(argc != 3) {
        fprintf(stderr, "Usage %s (host ip) (port number)\n", argv[0]);
        return -1;
    }

    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        err_exit("socket error");

    memset(&echo_server_addr, 0, sizeof(echo_server_addr));
    echo_server_addr.sin_family = AF_INET;
    echo_server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    echo_server_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr *)&echo_server_addr,
               sizeof(echo_server_addr)) < 0) {
        err_exit("connect error");
    }

    gettimeofday(&start, NULL);
    total = 0;
    while((recv_size = recv(sock, buf, BUFFSIZE, 0)) > 0) {
        total += recv_size;
    }
    gettimeofday(&end, NULL);
    double length = (double) (end.tv_sec-start.tv_sec)
        + (end.tv_usec-start.tv_usec) * 0.001 * 0.001;

    printf("%d %.1lf %.1lf\n",
            total, length, (double)total*8.0/length/1000000);

    close(sock);
    return 0;
}
