#include "PNtype.h"

#include <boost/noncopyable.hpp>

class PNEventLoop;//前置声明
/*******************************************************

每个Channel自始自终只服务于一个FD,  同时也只属于一个eventloop

********************************************************/

class PNEvent : boost::noncopyable{
public:

    typedef std::function<void()> CallbackFunctor;


    PNEvent();
    ~PNEvent();

    inline int getEventFD() const; //提供对fd访问接口

    void handleFunc(); //处理相应event

    inline void setReadCallback(CallbackFunctor ); //设置相应的 callback函数
    inline void setWriteCallback(CallbackFunctor ); ///想在后期增加一个接受右值引用的函数
    inline void setErrorCallback(CallbackFunctor );

    inline uint32_t getEvent() const; //对event事件的访问与修改
    inline void setRevent(uint32_t );

    inline weak_ptr<PNEventLoop> getOwnerLoop() const; // 提供对所属eventloop的访问
    void removeFromEventLoop(); //从eventloop中删除


private:
    void update();//当需要管理的event改变时, 需要对event_变量进行修改,

private:
    int eventFD_;
    std::weak_ptr<PNEventLoop> eventLoop_; //只为了访问到自己属于的loop对象, 不管理loop对象的析构操作,

    static int NoneEventFlag; //这里moduo定义成了私有, 但agilNet却定义成共有, 不过感觉这一部分外部不需要吧
    static int readEventFlag;
    static int writeEventFlag;
    static int errorEventFlag;

    uint32_t event_;
    uint32_t revent_;

    CallbackFunctor readCallback_;
    CallbackFunctor writeCallback_;
    CallbackFunctor errorCallback_;
};

int PNEvent::getEventFD() const{
    return eventFD_;
}

void PNEvent::setReadCallback(CallbackFunctor func){
    readCallback_ = func;
}

void PNEvent::setWriteCallback(CallbackFunctor func){
    writeCallback_ = func;
}

void PNEvent::setErrorCallback(CallbackFunctor func){
    errorCallback_ = func;
}

uint32_t PNEvent::getEvent() const{
    return event_;
}

void PNEvent::setRevent(uint32_t revt){
    revent_ = revt;
}

std::weak_ptr<PNEventLoop> PNEvent::getOwnerLoop() const{
    return eventLoop_;
}
