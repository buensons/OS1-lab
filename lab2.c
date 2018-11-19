
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#define ERROR(src) (perror(src), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))

void usage(char *p) {
    fprintf(stderr, "USAGE: %s \n", p);
    exit(EXIT_FAILURE);
}

void set_handler(void (*f)(int), int sig) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = f;
    if(-1 == sigaction(sig, &sa, NULL)) ERROR("sigaction");
}

void handle(int sig) {

}

void child_handle(int sig) {
    id_t pid;
    for(;;) {
        pid = waitpid(0, NULL, WNOHANG);
        if(pid == 0) {
            break;
        }
        if(pid < 0) {
            if(errno == ECHILD) break;
            ERROR("waitpid");
        }
}
}

void child_work(char * mark, int delay) {
    printf("%s %d\n", mark, delay);
}

int main(int argc, char ** argv) {
    int m, d;
    char *Fm;
    char *Fd;
    set_handler(child_handle, SIGCHLD);
    ssize_t count1, count2;
    if(argc != 3) usage(argv[0]);
    Fm = argv[1];
    Fd = argv[2];
    char buff1[1024];
    char buff2[1024];
    if((m = open(Fm, O_RDONLY)) < 0) ERROR("open");
    if((d = open(Fd, O_RDONLY)) < 0) ERROR("open");
    if((count1 = read(m, buff1, 1024)) < 0) ERROR("read");
    if((count2 = read(d, buff2, 1024)) < 0) ERROR("read");

    int delay[100];
    int i = 0;
    while(buff[i] != NULL) {
        delay[i] = atoi(buff2); // how to get just get one line at a time?
    }

    pid_t pid;
    for(int j = 0; j < child_no; j++) {
        if((pid = fork()) < 0) ERROR("fork");
        if(pid == 0) {
            child_work(buff[j], delay[j]);
            exit(EXIT_SUCCESS);
        }
    }

    close(m);
    close(d);
    while(wait(NULL) > 0);
    return EXIT_SUCCESS;
}