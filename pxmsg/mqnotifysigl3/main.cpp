#include <iostream>
#include "unpipc.h"

volatile sig_atomic_t mqflag;
static void sig_usr1(int signo){
    mqflag = 1;
    return;
}

int main(int argc, char** argv) {
    mqd_t mqd;
    void *buff;
    ssize_t n;
    sigset_t zeromask, newmask, oldmask;
    struct mq_attr attr;
    struct sigevent sigev;
    if (argc != 2){
        err_quit("usage: mqnotifysig3 <name>");
    }
    mqd = Mq_open(argv[1], O_RDONLY|O_NONBLOCK);
    Mq_getattr(mqd, &attr);
    buff = Malloc(attr.mq_msgsize);
    Sigemptyset(&zeromask);
    Sigemptyset(&newmask);
    Sigemptyset(&oldmask);
    Sigaddset(&newmask, SIGUSR1);
    Signal(SIGUSR1, sig_usr1);
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    Mq_notify(mqd, &sigev);
    for (;;){
        Sigprocmask(SIG_BLOCK, &newmask, &oldmask); ///block usr1
        while (mqflag == 0){
            sigsuspend(&zeromask);
        }
        mqflag = 0; ///reset flag
        Mq_notify(mqd, &sigev);
        while ((n= mq_receive(mqd, static_cast<char*>(buff), attr.mq_msgsize, nullptr)) >= 0){
            printf("read %ld bytes\n",  (long)n);
        }
        if (errno != EAGAIN){
            err_sys("mq_receive error");
        }
        Sigprocmask(SIG_UNBLOCK, &newmask, nullptr);///unblock usr1
    }
    return 0;
}
