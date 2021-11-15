#include <iostream>
#include "unpipc.h"

void client(int, int), server(int, int);

int main() {
    int pipe1[2], pipe2[2];
    pid_t childpid;
    ///create two pipes
    Pipe(pipe1);
    Pipe(pipe2);
    if (childpid=Fork(); childpid==0){///child
        Close(pipe1[1]);
        Close(pipe2[0]);
        server(pipe1[0], pipe2[1]); ///read from pipe1[0], write to pipe2[1]
        exit(0);
    }
    ///parent
    Close(pipe1[0]);
    Close(pipe1[1]);
    client(pipe2[0], pipe1[1]);
    Waitpid(childpid, nullptr, 0);

    return 0;
}
