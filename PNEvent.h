#ifndef PNEVENT_H
#define PNEVENT_H

#include "PNtype.h"

#include <boost/noncopyable.hpp>

class PNEventLoop;//前置声明
class PNTimestamp;
/*******************************************************

每个Channel自始自终只服务于一个FD,  同时也只属于一个eventloop

********************************************************/

class PNEvent : boost::noncopyable{
public:

    typedef std::function<void()> CallbackFunctor;
    typedef std::function<void(PNTimestamp)> ReadEventCallback;


    PNEvent(PNEventLoop*, int ); //创建 Event时需要提供两个参数, 一个为属于的Eventloop, 另一个为fd,
    ~PNEvent();

    inline int getEventFD() const; //提供对fd访问接口

    void handleFunc(PNTimestamp time); //处理相应event

    inline void setReadCallback(const ReadEventCallback&); //设置相应的 callback函数
    inline void setWriteCallback(const CallbackFunctor&); ///想在后期增加一个接受右值引用的函数
    inline void setErrorCallback(const CallbackFunctor&);
    inline void setCloseCallback(const CallbackFunctor&);

    inline uint32_t getEvent() const; //对event事件的访问与修改
    inline void setRevent(uint32_t );

    inline PNEventLoop* getOwnerLoop() const; // 提供对所属eventloop的访问
    void removeFromEventLoop(); //从eventloop中删除

    inline void enableReading();
    inline void disableReading();

    inline void enableWriting();
    inline void disableWriting();

    /*inline void enableError(); //这两个暂时不知道有没有什么意义
    inline void disableError();*/

    inline void disableAll();
    inline bool isNoneEvent();

private:
    void update();//当需要管理的event改变时, 需要对event_变量进行修改,

private:
    PNEventLoop* eventLoop_; //只为了访问到自己属于的loop对象, 不管理loop对象的析构操作,
    int eventFD_;
    bool addinLoop;

    static const uint32_t NoneEventFlag; //这里moduo定义成了私有, 但agilNet却定义成共有, 不过感觉这一部分外部不需要吧
    static const uint32_t readEventFlag;
    static const uint32_t writeEventFlag;
    static const uint32_t errorEventFlag;

    uint32_t event_;
    uint32_t revent_;

    bool eventHandling_;
    ReadEventCallback readCallback_;
    CallbackFunctor writeCallback_;
    CallbackFunctor errorCallback_;
    CallbackFunctor closeCallback_;
};

int PNEvent::getEventFD() const{
    return eventFD_;
}

void PNEvent::setReadCallback(const ReadEventCallback &func){
    readCallback_ = func;
}

void PNEvent::setWriteCallback(const CallbackFunctor &func){
    writeCallback_ = func;
}

void PNEvent::setErrorCallback(const CallbackFunctor &func){
    errorCallback_ = func;
}

void PNEvent::setCloseCallback(const CallbackFunctor &func){
    closeCallback_ = func;
}

uint32_t PNEvent::getEvent() const{
    return event_;
}

void PNEvent::setRevent(uint32_t revt){
    revent_ = revt;
}

void PNEvent::enableReading(){
    event_ |= readEventFlag;
    update();
}

void PNEvent::enableWriting(){
    event_ |= writeEventFlag;
    update();
}

void PNEvent::disableReading(){
    event_ &= ~readEventFlag;
    update();
}

void PNEvent::disableWriting(){
    event_ &= ~writeEventFlag;
    update();
}

void PNEvent::disableAll(){
    event_ &= NoneEventFlag;
}

bool PNEvent::isNoneEvent(){
    return event_ == NoneEventFlag;
}

PNEventLoop* PNEvent::getOwnerLoop() const{
    return eventLoop_;
}

#endif // PNEVENT_H

