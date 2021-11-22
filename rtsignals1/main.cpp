#include <iostream>
#include "unpipc.h"
static void sig_rt(int signo, siginfo_t *info, void *context){
    printf("received signal #%d, code = %d, ival = %d\n",
           signo, info->si_code, info->si_value.sival_int);
}
int main() {
    pid_t pid;
    sigset_t newset;
    union sigval val{};
    printf("SIGRTMIN = %d, SIGRTMAX = %d\n", (int)SIGRTMIN, (int)SIGRTMAX);

    if ((pid = Fork()) == 0){///child
        ///child: block three realtime signals
        Sigemptyset(&newset);
        Sigaddset(&newset, SIGRTMAX);
        Sigaddset(&newset, SIGRTMAX-1);
        Sigaddset(&newset, SIGRTMAX-2);
        Sigprocmask(SIG_BLOCK, &newset, nullptr);
        //establish signal handler with SA_SIGINFO set
        Signal_rt(SIGRTMAX, sig_rt);
        Signal_rt(SIGRTMAX-1, sig_rt);
        Signal_rt(SIGRTMAX-2, sig_rt);
        sleep(6);
        Sigprocmask(SIG_UNBLOCK, &newset, nullptr);
        sleep(3);
        exit(0);
    }
    ///parent sends nine signals to child
    sleep(3);
    for (int i = SIGRTMAX; i >= SIGRTMAX-2; --i) {
        for (int j = 0; j <= 2; ++j) {
            val.sival_int = j;
            Sigqueue(pid, i, val);
            printf("sent signal %d, val = %d\n", i, j);
        }
    }
    Waitpid(pid, nullptr, 0);
    return 0;
}

