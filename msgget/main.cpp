#include <iostream>
#include <sys/shm.h>
#include "unpipc.h"
int main() {
    int msqid;
    for (int i = 0; i < 10; ++i) {
        msqid = Msgget(IPC_PRIVATE, SVMSG_MODE|IPC_CREAT);
        std::cout << "msqid = " << msqid << '\n';
        Msgctl(msqid, IPC_RMID, nullptr);
    }
    return 0;
}
