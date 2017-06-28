#ifndef PNACCEPTOR_H
#define PNACCEPTOR_H

#include <boost/noncopyable.hpp>
#include <functional>

#include "PNEvent.h"


class PNEventLoop;

class PNAcceptor : boost::noncopyable{
public:
    typedef std::function<void(int sockfd, const )>
private:
};


#endif // PNACCEPTOR_H
