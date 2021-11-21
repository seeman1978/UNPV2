#include <iostream>
#include "unpipc.h"
#include <mqueue.h>
int main(int argc , char** argv) {
    int signo;
    mqd_t mqd;
    void *buff;
    ssize_t n;
    sigset_t newmask;
    struct mq_attr attr;
    struct sigevent sigev;
    if (argc != 2){
        err_quit("usage: mqnotifysig4 <name>");
    }
    mqd = Mq_open(argv[1], O_RDONLY|O_NONBLOCK);
    Mq_getattr(mqd, &attr);
    buff = Malloc(attr.mq_msgsize);

    Sigemptyset(&newmask);
    Sigaddset(&newmask, SIGUSR1);
    Sigprocmask(SIG_BLOCK, &newmask, nullptr);
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    Mq_notify(mqd, &sigev);

    for (;;){
        Sigwait(&newmask, &signo);
        if (signo == SIGUSR1){
            Mq_notify(mqd, &sigev);
            while((n=mq_receive(mqd, static_cast<char*>(buff), attr.mq_msgsize, nullptr)) >= 0){
                printf("read %ld bytes\n", (long)n);
            }
            if (errno != EAGAIN){
                err_sys("mq_receive error");
            }
        }
    }
    return 0;
}
