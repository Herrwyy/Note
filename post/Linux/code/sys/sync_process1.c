#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int n = 0;
int ppid, cpid;
void sys_err(char *str) {
    perror(str);
    exit(1);
}

void do_sigchild(int signo) {
    printf("I am child %d\t%d\n", getpid(), n);
    n += 2;
    kill(ppid, SIGUSR2);
}

void do_sigparent(int signo) {
    printf("I am parent %d\t%d\n", getpid(), n);
    n += 2;
    kill(cpid, SIGUSR1);
}

int main() {
    pid_t pid;
    struct sigaction act;
    if ((pid = fork()) < 0) {
        sys_err("fork");
    }else if (pid > 0) { 
        ppid = getpid();
        cpid = pid;
        n = 1;
        sleep(1);
        act.sa_handler = do_sigparent;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR2, &act, NULL);
        do_sigparent(0);
        while (1);
    }else if(pid == 0){
        cpid = getpid();
        ppid = getppid();
        n = 2;
        act.sa_handler = do_sigchild;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);
        while (1);
    }
    return 0;
}
