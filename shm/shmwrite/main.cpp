#include <sys/mman.h>
#include "unpipc.h"
int main(int argc, char** argv) {
    int fd;
    struct stat stat{};
    unsigned char *ptr;
    if (argc != 2){
        err_quit("usage: shmwrite <name>");
    }
    fd = Shm_open(argv[1], O_RDWR, FILE_MODE);
    Fstat(fd, &stat);
    ptr = static_cast<unsigned char *>(Mmap(nullptr, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    Close(fd);
    for (int i = 0; i < stat.st_size; ++i) {
        *ptr++ = i%256;
    }
    return 0;
}
