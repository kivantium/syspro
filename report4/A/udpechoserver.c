#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in addr;
    struct sockaddr_in senderinfo;
    socklen_t addrlen;
    char buf[2048];
    int n;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s (port number)\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    while (1) {
        memset(buf, 0, sizeof(buf));
        addrlen = sizeof(senderinfo);
        if((n = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                (struct sockaddr *)&senderinfo, &addrlen)) < 0) {
            perror("error in recvfrom");
            return -1;
        }
        if(sendto(sock, buf, n, 0, 
                  (struct sockaddr *)&senderinfo, addrlen) < 0) {
            perror("error in sendto");
            return -1;
        }
    }
    close(sock);
    return 0;
}
