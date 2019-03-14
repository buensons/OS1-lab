#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
                     exit(EXIT_FAILURE))

void child_work(int * fd_1, int * fd_2)
{
    srand(getpid());
    int ret;
    int value;
    do {
        if((ret = read(fd_1[0], &value, sizeof(value))) < 0) ERR("read");
        else if(ret == 0) {
            printf("child ret is 0\n");
            return;
        }
        else {
            if(value == 0) return;
            printf("CHILD PID: %d | %d\n", getpid(), --value);
            if(write(fd_2[1], &value, sizeof(value)) < sizeof(value)) ERR("write");
        }
    } while(value > 0);
    close(fd_1[0]);
    close(fd_2[1]);
}

void parent_work(int * fd_1, int * fd_2)
{
    srand(getpid());
    int ret;
    int value;
    do {
        if((ret = read(fd_2[0], &value, sizeof(value))) < 0) ERR("read");
        else if(ret == 0) {
            printf("parent ret is 0\n");
            return;
        }
        else {
            if(value == 0) return;
            printf("PARENT PID: %d | %d\n", getpid(), --value);
            if(write(fd_1[1], &value, sizeof(value)) < sizeof(value)) ERR("write");
        }
    } while(value > 0);
}

int main(int argc, char ** argv)
{
    int fd_1[2];
    int fd_2[2];
    int ret;

    if(argc != 2) ERR("usage");
    int value = atoi(argv[1]);

    if((ret = pipe(fd_1)) < 0) ERR("pipe");
    if((ret = pipe(fd_2)) < 0) ERR("pipe");
    if((ret = fork()) < 0) ERR("fork");
    else if(ret == 0) {
        //child
        close(fd_1[1]);
        close(fd_2[0]);
        child_work(fd_1, fd_2);
        exit(0);
    } else {
        //parent
        close(fd_1[0]);
        close(fd_2[1]);
        if((ret = write(fd_1[1], &value, sizeof(value))) < sizeof(value))
            ERR("write");

        parent_work(fd_1, fd_2);
        close(fd_1[1]);
        close(fd_2[0]);
    }
    wait(NULL);
    return 0;
}
