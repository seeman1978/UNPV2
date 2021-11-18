#include <iostream>
#include "unpipc.h"
constexpr auto SERV_FIFO = "/tmp/fifo.serv";
int main() {
    int readfifo, writefifo;
    size_t len;
    ssize_t n;
    char *ptr, fifoname[MAXLINE], buff[MAXLINE];
    pid_t pid;
    ///create fifo with our pid as part of name
    pid = getpid();
    snprintf(fifoname, sizeof fifoname, "/tmp/fifo.%ld", (long)pid);
    if (mkfifo(fifoname, FILE_MODE) < 0 && errno != EEXIST){
        err_sys("can't create %s", fifoname);
    }
    ///start buffer with pid and a blank
    snprintf(buff, sizeof buff, "%ld ", (long)pid);
    len = strlen(buff);
    ptr = buff + len;
    /// read pathname
    Fgets(ptr, MAXLINE - len, stdin);
    len = strlen(buff);
    writefifo = Open(SERV_FIFO, O_WRONLY, 0);
    Write(writefifo, buff, len);
    readfifo = Open(fifoname, O_RDONLY, 0);
    while ((n=Read(readfifo, buff, MAXLINE)) > 0){
        Write(STDOUT_FILENO, buff, n);
    }
    Close(readfifo);
    Unlink(fifoname);
    return 0;
}
