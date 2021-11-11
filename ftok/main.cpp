#include <iostream>
#include "unpipc.h"
int main(int argc, char** argv) {
    struct stat astat;
    if (argc != 2){
        err_quit("usage: ftok <pathname>");
    }
    Stat(argv[1], &astat);
    printf("st_dev: %lx, st_ino: %lx, key: %x\n", (u_long)astat.st_dev, (u_long)astat.st_ino, Ftok(argv[1], 0x57));
    return 0;
}
