#include <iostream>
#include "unpipc.h"

void client(int, int), server(int, int);

int main() {
    int pipe1[2], pipe2[2];
    pid_t childpid;
    Pipe(pipe1);
    Pipe(pipe2);
    return 0;
}
