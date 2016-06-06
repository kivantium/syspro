#include <stdio.h>

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <time.h>

struct linux_dirent {
   long           d_ino;
   off_t          d_off;
   unsigned short d_reclen;
   char           d_name[];
};

#define BUF_SIZE 1024*1024*5

// http://www.perlmonks.org/?node_id=1148448
int main(int argc, char **argv) {
   int fd, read_size, i;
   char buf[4096];
   struct linux_dirent *dir;
   char d_type;
   struct stat status;
   struct passwd *pass;
   struct group *grp;
   struct tm *time;

   if(argc != 1) {
       fprintf(stderr, "The number of argument is wrong");
       return -1;
    }

   fd = open(".", O_RDONLY | O_DIRECTORY);
   if (fd == -1) {
       perror("open error");
       return -1;
    }


   for (;;) {
       read_size = syscall(SYS_getdents, fd, buf, 4096);
       if (read_size == -1) {
           perror("getdents system call error");
           return -1;
       }

       if (read_size == 0) break;

       for (i=0; i<read_size;) {
           dir = (struct linux_dirent *) (buf + i);
           if (dir->d_ino != 0 && dir->d_name[0] != '.') {
               if(stat((const char *) dir->d_name,&status) < 0) {
                   perror("stat system call error");
                   return -1;
                }
               // http://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
               printf( (S_ISDIR(status.st_mode)) ? "d" : "-");
               printf( (status.st_mode & S_IRUSR) ? "r" : "-");
               printf( (status.st_mode & S_IWUSR) ? "w" : "-");
               printf( (status.st_mode & S_IXUSR) ? "x" : "-");
               printf( (status.st_mode & S_IRGRP) ? "r" : "-");
               printf( (status.st_mode & S_IWGRP) ? "w" : "-");
               printf( (status.st_mode & S_IXGRP) ? "x" : "-");
               printf( (status.st_mode & S_IROTH) ? "r" : "-");
               printf( (status.st_mode & S_IWOTH) ? "w" : "-");
               printf( (status.st_mode & S_IXOTH) ? "x" : "-");

               pass = getpwuid(status.st_uid);
               grp = getgrgid(status.st_gid);
               time = localtime(&status.st_ctim.tv_sec);
               printf(" %d %s %s %d %d月 %d %02d:%02d %s\n", (int) status.st_nlink, 
                       pass->pw_name, grp->gr_name, (int)status.st_size, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min, dir->d_name);
           }
           i += dir->d_reclen;
       }
   }

   return 0;
}
