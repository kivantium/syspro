#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int src, dst, tmp;
    ssize_t read_size, write_size;
    char buf[8192];
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
        return -1;
    }

    // open src file
    src = open(argv[1], O_RDONLY);
    if(src == -1) {
        perror("source file open error");
        return -1;
    }

    if(stat(argv[1], &status)==-1) {
        perror("error during stat");
        return -1;
    }

    // open dst file
    dst = open(argv[2], O_WRONLY|O_CREAT, status.st_mode);
    if(dst == -1) {
        perror("destination file open error");
        return -1;
    }
    
    while(1) {
        read_size = read(src, buf, 1);
        if(read_size == 0) break;
        else if(read_size == -1) perror("error reading file");
        write_size = 0;
        do {
            tmp = write(dst, buf+write_size, read_size-write_size);
            if(tmp == -1) {
                perror("error writing file");
                return -1;
            }
            write_size += tmp;
        } while(write_size < read_size);
    }
    gettimeofday(&end, NULL);
    printf("%ld\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec));
    return 0;
}
