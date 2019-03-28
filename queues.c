#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>

#define MAX_BUFFER 20

#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
                                     exit(EXIT_FAILURE))

void usage(void){
        fprintf(stderr,"USAGE: taskc name_of_the_server \n");
        exit(EXIT_FAILURE);
}

void child_work(mqd_t desc) {
    unsigned int prio;
    int ret;
    char buffer[MAX_BUFFER];
    for(;;) {
        
        if((ret = mq_receive(desc, buffer, MAX_BUFFER, &prio)) < 0)
        {
            if(errno != EAGAIN) ERR("receive");
            continue;
        }
        printf("Message read: %s \n", buffer);
    }
    mq_close(desc);
    exit(0);
}

int main(int argc, char ** argv)
{
    mqd_t desc;
    char name[MAX_BUFFER];
    char message[MAX_BUFFER];
    int ret;
    snprintf(name,MAX_BUFFER,"/mq_%d",(int)getpid());
    struct mq_attr attr;
    attr.mq_maxmsg = 3;
    attr.mq_msgsize = MAX_BUFFER;

    if((desc = mq_open(name, O_RDWR | O_CREAT, 0666, &attr)) < 0) ERR("open");

    if((ret = fork()) < 0)
    {
        ERR("fork");
    } else if(ret == 0) {
        child_work(desc);
    } else {
        for(;;)
        {
            scanf("%s", message);
            if((ret = mq_send(desc, message, MAX_BUFFER, 0)) < 0)
            {
                if(errno == EAGAIN){
                    printf("read something\n");
                    continue;
                }
                ERR("send");
            }
        }
    }
    mq_close(desc);
    mq_unlink(&name);
    return 0;

}