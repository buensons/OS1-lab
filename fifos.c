#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

void read_from_fifo(int fifo) {
    ssize_t count;
    char c;
    do {
        if((count = read(fifo, &c, 1)) < 0) {
            printf("read\n");
            exit(1);
        }
        if(count > 0 && isalnum(c)) printf("%c", c);
    } while(count > 0);
}

int main(int argc, char ** argv)
{
    int fd;

    if(argc != 2) {
        printf("argc\n");
        return 1;
    }

    if(mkfifo(argv[1], S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))
    {
        if(errno != EEXIST) {
            printf("mkfifo\n");
            return 1;
        }
    }

    if((fd = open(argv[1], O_RDONLY)) < 0)
    {
        printf("open\n");
        return 1;
    }

    read_from_fifo(fd);
    if(close(fd) < 0) {
        printf("close");
        return 1;
    }
    return 0;
}