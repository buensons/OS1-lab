#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#define ERROR(src) (perror(src), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))

void worker(int n, int * fd_read_parent, int * fd_write_parent) 
{
    int ret, value;
    int fd_read_child[2];
    int fd_write_child[2];
    close(fd_read_parent[1]);
    close(fd_write_parent[0]);
    printf("STARTED: PID: %d PPID: %d\n", getpid(), getppid());

    if(n != 0) 
    {
        if((ret = pipe(fd_read_child)) < 0) ERROR("pipe");
        if((ret = pipe(fd_write_child)) < 0) ERROR("pipe");

        if((ret = read(fd_read_parent[0], &value, sizeof(value))) < sizeof(value)) ERROR("read");

        printf("CURR: %d\n", ++value);

        if((ret = write(fd_write_child[1], &value, sizeof(value))) < sizeof(value)) ERROR("write");

        if((ret = fork()) < 0) ERROR("fork");
        else if(ret == 0) 
            worker(--n, fd_write_child, fd_read_child);
        else {
            close(fd_read_child[1]);
            close(fd_write_child[0]);
            wait(NULL);
            if((ret = read(fd_read_child[0], &value, sizeof(value))) < sizeof(value)) ERROR("read");
            if((ret = write(fd_write_parent[1], &value, sizeof(value))) < sizeof(value)) ERROR("write");
            
            close(fd_read_parent[0]);
            close(fd_write_parent[1]);
            close(fd_read_child[0]);
            close(fd_write_child[1]);
            printf("FINISHING: PID: %d PPID: %d\n", getpid(), getppid());
            exit(0);
        }
    } else {
        if((ret = read(fd_read_parent[0], &value, sizeof(value))) < sizeof(value)) ERROR("read");
        printf("CURR: %d\n", ++value);
        if((ret = write(fd_write_parent[1], &value, sizeof(value))) < sizeof(value)) ERROR("write");
        
        sleep(1);
        close(fd_read_parent[0]);
        close(fd_write_parent[1]);
        printf("FINISHING: PID: %d PPID: %d\n", getpid(), getppid());
        exit(0);
    }
}

int main(int argc, char ** argv) 
{
    int n, ret;
    int fd_write[2];
    int fd_read[2];
    int value = 0;

    if(argc != 2) ERROR("argc");

    n = atoi(argv[1]);
    if(n < 1 || n > 20) ERROR("args");

    if((ret = pipe(fd_write)) < 0) ERROR("pipe");
    if((ret = pipe(fd_read)) < 0) ERROR("pipe");

    if((ret = fork()) < 0) ERROR("fork");
    else if(ret == 0)
        worker(--n, fd_write, fd_read);
    else {
        //main process work
        close(fd_write[0]);
        close(fd_read[1]);
        
        if((ret = write(fd_write[1], &value, sizeof(value))) < sizeof(value)) ERROR("write");
        printf("MAIN: PID: %d PPID: %d\n", getpid(), getppid());
        
        wait(NULL);
        if((ret = read(fd_read[0], &value, sizeof(value))) < sizeof(value)) ERROR("read");

        close(fd_write[1]);
        close(fd_read[0]);
        printf("VALUE: %d\n", value);
        printf("finishing main process...\n");
    }
    return 0;
}