#include "unpipc.h"
#include <mqueue.h>
int main(int argc, char** argv) {
    int c, flags;
    mqd_t mqd;
    flags = O_RDWR | O_CREAT;
    while ((c = Getopt(argc, argv, "e")) != -1){
        switch (c) {
            case 'e':
                flags |= O_EXCL;
                break;
        }
    }
    if (optind != argc -1){
        err_quit("usage: mqcreate [-e] <name>");
    }
    mqd = Mq_open(argv[optind], flags, FILE_MODE, NULL);
    struct mq_attr attr;
    Mq_getattr(mqd, &attr);
    printf("max msgs = %ld, max bytes/msgs = %ld, current on queue = %ld\n", attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
    Mq_close(mqd);
    return 0;
}
