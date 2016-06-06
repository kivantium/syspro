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
    socklen_t senderinfolen;
    int n;
    char buf_send[1024], buf_recv[1024];

    if(argc != 3) {
        fprintf(stderr, "Usage %s (host ip) (port number)\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    while(fgets(buf_send, sizeof(buf_send), stdin) != NULL) {
        if(sendto(sock, buf_send, strlen(buf_send), 0, 
                        (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        return 1;
    }
        memset(buf_recv, 0, sizeof(buf_recv));
        senderinfolen = sizeof(senderinfo);
        recvfrom(sock, buf_recv, sizeof(buf_recv), 0,
                (struct sockaddr *)&senderinfo, &senderinfolen);
        printf("%s", buf_recv);
    }
    close(sock);
    return 0;
}
