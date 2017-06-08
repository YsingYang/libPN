#ifndef PNEPOLL_H
#define PNEPOLL_H

#include <boost/noncopyable.hpp>
#include <sys/epoll.h>
#include <map>
#include <vector>

class PNEventLoop;
class PNEvent;

class PNEpoll : boost::noncopyable{
public:
    PNEpoll(PNEventLoop* );
    ~PNEpoll();

    void poll(std::vector<PNEvent*> &activeEventList);

    bool addEvent(PNEvent* event);//添加event;
    bool removeEvent(PNEvent* event);
    bool updateEvent(PNEvent* event);

private:
    static const int InitEventListSize = 16;//const变量可以类内初始化

    void fillActivEventsList(int numsActiveEvents, std::vector<PNEvent*>& activeEventList);//将活跃的事件还给EventLoop

    int epollFD_;
    std::map<int, PNEvent*> eventMap_;//给出一个fd->event的map
    std::vector<struct epoll_event>eventList_;//用于wait后得到就绪的event集合

    PNEventLoop* ownerLoop_;
};


#endif // PNEPOLL_H
