#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int tmp;
    ssize_t read_size, write_size;
    char buf[8192];
    FILE *src, *dst;
    struct stat status;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    // argument check
    if(argc != 3) {
        fprintf(stderr, "Argument number error.\nUsage: mycp from to\n");
    }

    // check whether two argument is the same
    if(strcmp(argv[1], argv[2]) == 0) {
        fprintf(stderr, "mycp: `%s\' と `%s\' は同じファイルです\n",
                argv[1], argv[2]);
    }

    // open src file
    if((src = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "source file open error\n");
        return -1;
    }


    // open dst file
    if((dst = fopen(argv[2], "w")) == NULL) {
        fprintf(stderr, "destination file open error\n");
    }
    
    while(1) {
        read_size = fread(buf, sizeof(char), 1, src);
        if(read_size == 0) break;
        else if(read_size == -1) fprintf(stderr, "error reading file");
        tmp = fwrite(buf, sizeof(char),  read_size, dst);
        if(tmp < read_size) {
            fprintf(stdout, "error writing file");
            return -1;
        }
    }
    gettimeofday(&end, NULL);
    printf("%ld\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec));
    return 0;
}
