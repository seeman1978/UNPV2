#include <mqueue.h>
#include "unpipc.h"

int pipefd[2];

static void sig_usr1(int){
    Write(pipefd[1], (void *) "0", 1);
    return;
}

int main(int argc, char** argv) {
    int nfds;
    char c;
    fd_set rset;
    mqd_t mqd;
    void *buff;
    ssize_t n;
    struct mq_attr attr{};
    struct sigevent sigev{};
    if (argc != 2){
        err_quit("usage mqnotifysig5 <name>");
    }
    mqd = Mq_open(argv[1], O_RDONLY|O_NONBLOCK);
    Mq_getattr(mqd, &attr);
    buff = Malloc(attr.mq_msgsize);

    Pipe(pipefd);

    Signal(SIGUSR1, sig_usr1);
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    Mq_notify(mqd, &sigev);
    FD_ZERO(&rset);
    for (;;){
        FD_SET(pipefd[0], &rset);
        nfds = Select(pipefd[0]+1, &rset, nullptr, nullptr, nullptr);
        if (FD_ISSET(pipefd[0], &rset)){
            Read(pipefd[0], &c, 1);
            Mq_notify(mqd, &sigev);
            while ((n= mq_receive(mqd, static_cast<char*>(buff), attr.mq_msgsize, nullptr)) >= 0){
                printf("read %ld bytes\n", (long)n);
            }
            if (errno != EAGAIN){
                err_sys("mq_receive error");
            }
        }
    }
    return 0;
}
