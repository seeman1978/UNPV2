#include <iostream>
#include "unpipc.h"
mqd_t mqd;
void *buff;
struct mq_attr attr;
struct sigevent sigev;
static void sig_usr1(int signo){
    ssize_t n;
    Mq_notify(mqd, &sigev); ///register first
    n = Mq_receive(mqd, static_cast<char*>(buff), attr.mq_msgsize, nullptr);
    printf("SIGUSR1 RECEIVED, READ %ld bytes\n", (long)n);
    return;
}

int main(int argc, char** argv) {
    if (argc != 2){
        err_quit("usage: mqnotifysigl <name>");
    }
    ///open queue, get attribute , allocate read buffer
    mqd = Mq_open(argv[1], O_RDONLY);
    Mq_getattr(mqd, &attr);
    buff = Malloc(attr.mq_msgsize);
    ///establish signal handler, enable notification
    Signal(SIGUSR1, sig_usr1);
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    Mq_notify(mqd, &sigev);
    for (;;){
        pause();    ///signal handler does everything
    }
    return 0;
}
