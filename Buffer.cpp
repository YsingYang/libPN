#include "Buffer.h"
#include <memory.h>
#include <sys/uio.h>


ssize_t Buffer::readFd(int fd, int* savedError){
    char extrabuf[65536];
    struct iovec vec[2]; //还没用过这种结构...
    const size_t writeable = writeableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writeable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const ssize_t n = readv(fd, vec, 2);
    if(n < 0){
        *savedError = errno;
    }
    else if(static_cast<size_t>(n) <= writeable){
        writerIndex_ += n;
    }
    else{
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writeable);
    }
    return n;
}
