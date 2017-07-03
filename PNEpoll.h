#ifndef PNEPOLL_H
#define PNEPOLL_H

#include <boost/noncopyable.hpp>
#include <sys/epoll.h>
#include <map>
#include <vector>
#include "PNEventLoop.h"


class PNEvent;

class PNEpoll : boost::noncopyable{
public:
    PNEpoll(PNEventLoop* );
    ~PNEpoll();

    PNTimestamp poll(int timeoutMs, std::vector<PNEvent*> &activeEventList);

    void addEvent(PNEvent* event);//添加event;
    void  removeEvent(PNEvent* event);
    void  updateEvent(PNEvent* event);

    inline void assertInLoopThread() const;

private:

    void fillActivEventsList(int numsActiveEvents, std::vector<PNEvent*>& activeEventList);//将活跃的事件还给EventLoop
    void update(int operation, PNEvent* );


private:
    static const int InitEventListSize = 16;//const变量可以类内初始化

    int epollFD_;
    std::map<int, PNEvent*> eventMap_;//给出一个fd->event的map
    std::vector<struct epoll_event>eventList_;//用于wait后得到就绪的event集合

    PNEventLoop* ownerLoop_;
};


void PNEpoll::assertInLoopThread() const{
    ownerLoop_ ->assertInLoopThread();
}

#endif // PNEPOLL_H
