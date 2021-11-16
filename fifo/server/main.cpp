#include <iostream>
#include "unpipc.h"
void server(int , int);
constexpr auto FIFO1 = "/tmp/fifo.1";
constexpr auto FIFO2 = "/tmp/fifo.2";
int main() {
    int readfd, writefd;
    if (mkfifo(FIFO1, FILE_MODE) < 0 && errno != EEXIST){
        err_sys("can't create %s", FIFO1);
    }
    if (mkfifo(FIFO2, FILE_MODE) < 0 && errno != EEXIST){
        Unlink(FIFO1);
        err_sys("can't create %s", FIFO2);
    }
    readfd = Open(FIFO1, O_RDONLY, 0);
    writefd = Open(FIFO2, O_WRONLY, 0);
    server(readfd, writefd);
    return 0;
}
