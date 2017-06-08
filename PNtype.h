#ifndef PNTYPE_H
#define PNTYPE_H

#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <set>
#include <memory>
#include <functional>
#include <thread>


namespace{
    typedef std::thread::id Tid;

    const int BUFFLEN = 1024;
    const int MAXEVENTS = 4096;
}



#endif // PNTYPE_H
