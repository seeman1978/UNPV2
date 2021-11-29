#include <iostream>
#include "unpipc.h"
int main() {
    int fd;
    fd = Open("test1.data", O_RDWR|O_CREAT, FILE_MODE);
    Write_lock(fd, 0, SEEK_SET, 0); ///PARENT WRITE LOCKS ENTIRE FILE
    std::cout << Gf_time << ": parent has write lock\n";
    if (Fork() == 0){//second child
        sleep(1);
        std::cout << Gf_time << ": second child tries to obtain read lock\n";
        Readw_lock(fd, 0, SEEK_SET, 0);///this should block
        std::cout << Gf_time << ": second child obtains read lock\n";
        sleep(4);
        Un_lock(fd, 0, SEEK_SET, 0);
        std::cout << Gf_time << ": second child releases read lock\n";
        exit(0);
    }

    if (Fork() == 0){//first child
        sleep(3);
        std::cout << Gf_time << ": first child tries to obtain write lock\n";
        Writew_lock(fd, 0, SEEK_SET, 0);///this should block
        std::cout << Gf_time << ": first child obtains write lock\n";
        sleep(2);
        Un_lock(fd, 0, SEEK_SET, 0);
        std::cout << Gf_time << ": first child releases write lock\n";
        exit(0);
    }

    ///parent
    sleep(5);
    Un_lock(fd, 0, SEEK_SET, 0);
    std::cout << Gf_time << ": parent releases write lock\n";
    sleep(10);
    return 0;
}
