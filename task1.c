
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#define ERROR(src) (perror(src), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t last_signal = 0;
volatile sig_atomic_t sig2 = 0;

void usage(char *p) {
    fprintf(stderr, "USAGE: %s [number of processes]\n", p);
    exit(EXIT_FAILURE);
}

void set_handler(void (*f)(int), int sig) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = f;
    if(-1 == sigaction(sig, &sa, NULL)) ERROR("sigaction");
}

void sig_handler(int sig) {
    printf("%d received a signal %d\n", getpid(), sig);
    last_signal = sig;
    if(sig == SIGUSR2) {
        sig2++;
    }
}

void sigchld_handler(int sig) {
    pid_t pid;
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


void child_work(int m, int n) {
    struct timespec t = {0, m*10000};
    int iterator = 0;
    int sig2_counter = 0;
    while(1) {
        nanosleep(&t, NULL);
        if(++iterator % n == 0) {
            kill(getppid(), SIGUSR2);
            sig2_counter++;
            printf("Child have sent %d SIGUSR2 already\n", sig2_counter);
        } else {
            kill(getppid(), SIGUSR1);
        }
    
    }
}

void create_child(int m, int n) {
        switch(fork()) {
            case 0:
                child_work(m, n);
                exit(EXIT_SUCCESS);
            case -1:
                ERROR("fork");
        }
}

void parent_work(sigset_t old_mask) {
    while(1) {
        last_signal = 0;
        while(last_signal != SIGUSR2) {
            sigsuspend(&old_mask);
            printf("[PARENT] Number of SIG2 received: %d\n", sig2);
        }
    }
}

// Program starts one child process, which sends every "m" (parameter) microseconds a SIGUSR1 signal to the parent.
// Every n-th signal is changed to SIGUSR2. Parent anticipates SIGUSR2 and counts the amount of signals received.
// Child process also counts the amount of SIGUSR2 sent.
// Both processes print out the counted amounts at each signal operation. We reuse some functions from previous code.

int main(int argc, char ** argv) {
    if(argc != 3) usage(argv[0]);
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    set_handler(sigchld_handler, SIGCHLD);
    set_handler(sig_handler, SIGUSR1);
    set_handler(sig_handler, SIGUSR2);
    create_child(m, n);
    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);
    parent_work(old_mask);
    while(wait(NULL) > 0);
    sigprocmask(SIG_UNBLOCK, &old_mask, NULL);
    return EXIT_SUCCESS;
}