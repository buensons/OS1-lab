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
    int ret, res, index;
    do {
        char buff[16];
        if((ret = read(fd_1[0], buff, strlen(buff))) < 0) ERR("read");
        else if(ret == 0) return;
        else {
            index = rand() % strlen(buff);
            char new_string[ret];
            int j = 0;
            for(int i = 0; i < strlen(buff); i++) {
                if(i != index) {
                    new_string[j] = buff[i];
                    j++;
                }
            }
            printf("CHILD PID: %d | %s\n", getpid(), new_string);
            if((res = write(fd_2[1], new_string, ret)) < ret) ERR("write");
        }
    } while(ret > 0);
    close(fd_1[0]);
    close(fd_2[1]);
}

void parent_work(int * fd_1, int * fd_2)
{
    srand(getpid());
    int ret, res, index;
    do {
        char buff[16];
        if((ret = read(fd_2[0], buff, strlen(buff))) < 0) ERR("read");
        else if(ret == 0) return;
        else {
            index = rand() % strlen(buff);
            char new_string[ret];
            int j = 0;
            for(int i = 0; i < strlen(buff) + 1; i++) {
                if(i != index) {
                    new_string[j] = buff[i];
                    j++;
                }
            }
            printf("PARENT PID: %d | %s\n", getpid(), new_string);
            
            if((res = write(fd_1[1], new_string, ret)) < ret) ERR("write");
        }
    } while(ret > 0);
}

int main(int argc, char ** argv)
{
    int fd_1[2];
    int fd_2[2];
    int ret;
    char * string;

    if(argc != 2) ERR("usage");

    string = argv[1];
    if((ret = pipe(fd_1)) < 0) ERR("pipe");
    if((ret = pipe(fd_2)) < 0) ERR("pipe");
    if((ret = fork()) < 0) ERR("fork");
    else if(ret == 0) {
        //child
        close(fd_1[1]);
        close(fd_2[0]);
        child_work(fd_1, fd_2);
    } else {
        //parent
        close(fd_1[0]);
        close(fd_2[1]);
        if((ret = write(fd_1[1], string, strlen(string))) < strlen(string))
            ERR("write");

        parent_work(fd_1, fd_2);
        close(fd_1[1]);
        close(fd_2[0]);
    }
    wait(NULL);
    return 0;
}