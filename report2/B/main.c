#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int src, dst, tmp;
    ssize_t read_size, write_size;
    char buf[256];
    struct stat status;

    // argument check
    if(argc != 3) {
        fprintf(stderr, "Argument number error.\nUsage: mycp from to\n");
        return -1;
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
        read_size = read(src, buf, 256);
        if(read_size == 0) break;
        else if(read_size == -1) {
            perror("error reading file");
            return -1;
        }
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
    return 0;
}
