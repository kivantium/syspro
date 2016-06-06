#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int src, dst, tmp, i;
    ssize_t read_size, write_size;
    char buf[256];
    struct stat status;
    int line = 0;
    int word = 0;
    int byte = 0;
    int isWord = 0;

    // argument check
    if(argc != 2) {
        fprintf(stderr, "Argument number error.\nUsage: mywc file\n");
    }

    // open src file
    src = open(argv[1], O_RDONLY);
    if(src == -1) {
        perror("source file open error");
    }

    while(1) {
        read_size = read(src, buf, 256);
        if(read_size == 0) break;
        else if(read_size == -1) perror("error reading file");

        byte += read_size;

        for(i=0; i<read_size; ++i) {
            if(buf[i] == '\n') line++;
            if(buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n') {
                isWord = 0;
            } else if(isWord == 0) {
                isWord = 1;
                word++;
            }
        }
    }
    printf("line: %d, word: %d, byte: %d\n", line, word, byte);

    return 0;
}
