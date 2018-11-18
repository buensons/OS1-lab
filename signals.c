#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define ERROR(src) (perror(src), kill(0, SIGINT), exit(EXIT_FAILURE))

volatile sig_atomic_t last_signal = 0;
volatile sig_atomic_t counter = 0;

void usage(char * p) {
    fprintf(stderr, "USAGE: %s [czas] [n]\n", p);
    exit(EXIT_FAILURE);
}

void set_handler(void (*f)(int), int sig) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = f;
    if(-1 == sigaction(sig, &sa, NULL)) ERROR("sigaction");
}

void sig_handle(int sig) {
    last_signal = sig;
    counter++;
    if(counter == 100) {
        kill(0, SIGUSR2);
    }
}


void sigchld_handler(int sig) {
    pid_t pid;
    for(;;) {
        pid = waitpid(0, NULL, WNOHANG);
        if(pid == 0) break;
        if(pid < 0) {
            if(errno == ECHILD) break;
            ERROR("waitpid");
        }
    }
}

void child_work() {
    srand(time(NULL) * getpid());
    int s = 100 + rand()%100;
    struct timespec ts = {0, s*10000};
    while(last_signal == 0) {
        nanosleep(&ts, NULL);
        printf("*\n");
        if(kill(getppid(), SIGUSR1)) ERROR("kill");
    }
}


void create_child(int n) {
    for(int i = 0; i < n; i++) {
        switch(fork()) {
            case 0:
                child_work();
                exit(EXIT_SUCCESS);
            case -1:
                ERROR("fork");
        }
    }
}

int main(int argc, char ** argv) {
    if(argc != 2) usage(argv[0]);
    int n = atoi(argv[1]);
    set_handler(sigchld_handler, SIGCHLD);
    set_handler(sig_handle, SIGUSR2);
    set_handler(sig_handle, SIGUSR1);
    create_child(n);

    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);

    while(counter < 100) {
        last_signal = 0;
        while(last_signal != SIGUSR1 && counter < 100) {
            sigsuspend(&old_mask);
            printf("Received %d signals already\n", counter);
        }
    }
    while(wait(NULL) > 0);
    sigprocmask(SIG_UNBLOCK, &old_mask, NULL);

    return EXIT_SUCCESS;
}