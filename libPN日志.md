## Date_1  2017. 6. 6
#### 想写一个网络库的缘由
   面试了TX, Ali后发现自己网络, OS方面还有点欠缺, 于是想和同学写一个小型IM系统来弥补一下自己的缺陷..可是发现自己写的网络模型有点糟糕...写到后面不知道从何下手, 虽说基本上能完成接收发送数据了, 足够实验室用了, 但是感觉自己并没有完全了解到什么, 之前知乎上看到一些大神推荐看看nginx, libevent, redis的源码, 刚好Ali面试后面试官也推荐我看看libevent, redis的源码, libevent也算是属于我现在想写的reactor模型的典范, 于是想参照相应的rector模型, 如muduo, libevent, 来写一个网络库
   
#### 问题
  参考别人的源码中发现agiNet中的EventLoop 好像拆分成了一个EventLoop与EventCtrl, 我觉得有好有不好, 这多一层封装上的意义其实不太大, 故跟muduo一样直接写成一个EventLoop里, 不知agiNet作者是否别有用意, 还是我理解有误
  

#### 进展
  基本完成EventLoop思路
   
   
## Date_2 2017. 6. 7
#### 今日任务
设计好相应的Event类, 最好把函数也实现完

#### 扩展思路
对于一些set操作, 能否添加一个重载右值引用的传递函数 , 如在setcallback那里


#### 学到一个好像很吊的东西
__thread关键字
1. __thread是GCC内置的线程局部存储设施。它的实现非常高效，比Pthread库中的pthread_key_t（muduo中ThreadLocal）快很多。
2. \__thread变量是表示每个线程有一份独立实体，各个线程的变量值互不干扰。__thread只能修饰POD类型，不能修饰class类型，因为无法自动调用构造函数和析构函数。

#### 进展
   基本完成了Event的设计, 不过由于后来看书, 然后手贱去测试 eventloop了, 不过学习到__thread这个东西还是不错的, 来时正好奇为什么muduo中,可以loopInthisThread这个变量, 明天需要把Event里的内容基本实现下
   
   

## Date_3 2017. 6. 8
#### 今日任务
   继续完成Event的设计


#### 任务进展
   基本完成了Event的设计, 在进行PNEpoll的设计, 下次可以开始从event->setRevent开始写起
   
#### 有一点问题是, 暂时不知道eventfd在什么地方close时, 暂时觉得是在remove的


## Date_4 2017. 6. 10
#### 吐槽
吐槽一下有限域老师讲课内容任务分配不均, 让我花费了许多时间..很烦...


#### 任务进展
   基本完成了epoll类的的设计, 但是要谐调管理好event->update方法, 
   
#### 下次工作
   完成event->update函数, 确保能吻合epoll->update的接口

## Date_5 2017.6 12
#### 吐槽++
   哈哈哈哈哈, 终于搞定完有限域啦哈哈哈哈.
   
#### 任务
   做好eventLoop中, 连接event 与 epoll的接口实现
   
#### 疑问
   1. 在aginNet中, 他在eventloop的eventctrl里还加上了个fd -> event的map, 但是对于这个map, 在epoll中已经有了, 我觉得这是个不必要的设计
   2. 记得之前提过一个类似的问题, epoll中的remove操作与add 操作只定义了移除/添加该fd的操作, 但并不管理event的析构, event的析构在哪里比较合理, 还是自己处理?

#### 任务进度
基本完成了eventLoop, event , epoll的设计, 通过test2的测试

#### 下次任务
添加timer计时器, 同时研究下test2下的 timer里面的一些参数

## Date_6 2017. 6. 14
实验室出去吃了顿好的~ 然后回来就没心情学习了(捂脸)

内容, 先了解之前test2中timeFD的东西, 因为timer类是基于其中的timer的
1. timerfd是Linux为用户程序提供的一个定时器接口。这个接口基于文件描述符，通过文件描述符的可读事件进行超时通知

```
int timerfd_create(int clockid, int flags);

int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);

int timerfd_gettime(int fd, struct itimerspec *curr_value);
```
#### itimerspec
```
struct itimerspec {
	struct timespec it_interval;  /* Interval for periodic timer */
    struct timespec it_value;     /* Initial expiration */
};
```
结构体itimerspec就是timerfd要设置的超时结构体，它的成员it_value表示定时器第一次超时时间，it_interval表示之后的超时时间即每隔多长时间超时
####timerfd_create
第一个参数：clockid指定时间类型，有两个值：

CLOCK_REALTIME :Systemwide realtime clock. 系统范围内的实时时钟

CLOCK_MONOTONIC:以固定的速率运行，从不进行调整和复位 ,它不受任何系统time-of-day时钟修改的影响

第二个参数：flags可以是0或者O_CLOEXEC/O_NONBLOCK。
(O_NONBLOCK即当调用exec（）函数成功后，文件描述符会自动关闭)

####timerfd_settime
作用：用来启动或关闭有fd指定的定时器

参数：

fd：timerfd，有timerfd_create函数返回

fnew_value:指定新的超时时间，设定new_value.it_value非零则启动定时器，否则关闭定时器，如果new_value.it_interval为0，则定时器只定时一次，即初始那次，否则之后每隔设定时间超时一次

old_value：不为null，则返回定时器这次设置之前的超时时间

flags：1代表设置的是绝对时间；为0代表相对时间。

int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);


####timerfd_gettime
int timerfd_gettime(int fd, struct itimerspec *curr_value);

此函数用于获得定时器距离下次超时还剩下的时间。如果调用时定时器已经到期，并且该定时器处于循环模式（设置超时时间时struct itimerspec::it_interval不为0），那么调用此函数之后定时器重新开始计时。



#### 总结
感觉timer类运用上接触得相对比较少, 所以还需要继续看一看, 基本实现上是参考着agiNet, muduo上的相对会复杂一点, 先照着实现一下, 看有没有什么地方可以改变


###Date-7 2017. 6. 15
碰到了一个之前都没有遇到类, 并且觉得里面有些实现实现得封装过度的感觉, 所以Timer的学习还是慢慢来吧, 
####TimeStamp
昨天照着agilNet的Timer的实现也并是不完全不用功, 至少现在看回timeStamp, 也发现其实就是一个对微妙的封装

先了解关于时间的一个结构体和获取时间的一个函数。
````
#include <time.h>
struct timeval  {  
    __time_t tv_sec;        /* Seconds. */  
    __suseconds_t tv_usec;  /* Microseconds. */  
}; 
````
tv_sec指从Epoc到现在的秒数。 
tv_usec指从Epoch到现在的时间 - 秒数 剩下的微秒数。 
Epoch指的是一个特定的时间：1970-01-01 00:00:00 UTC。

###问题
为什么源码中有个些比较的地方不用const 引用传参, 是因为TimeStamp作为一个int64_t长度的类, 直接构造会比引用传参快吗?

###Date-8 2017. 6. 16
####内容
1. 添加PNTimestamp的to_string显示操作, muduo的实现是利用sprintf, 应该会相对快一些, 反正是学习, 所以我用了boost::format函数格式化输出到stringstream中.

2. 基本通过了测试样例, 中间暂时没发现什么其他问题.

####明天任务
开始对TimerID,与Timer类的实现


###Date-9 2017. 6. 17
####一些问题
1. 关于muduo对于timer类的创建有个static 的计数器, 但他在base类里设计了一个该计数器线程安全的类, 我这里直接使用的是atomic<int>, 那么是否可以保证其线程安全	
2. 感觉interval那里有问题, 因为我觉得interval的本意是每超时一次, 减少一次interval , 只至为0 . 但是似乎程序表现出来的是, 如果设置了interval > 0, 将会无限制的重新设置超时时间

####工作进展
完成简单的timerid, timer 的类的设计, 不过muduo 源码中设计到多线程下的timer设计, 有点难理解, 我觉得可以先按照书本实现, 再慢慢理解

###Date-10 2017. 6. 22
考完试后浪了3天回来...发现之前写了什么都不太记得了...T^T, 

####疑问点
1. 关于PNTimerQueue::addTimerInLoop
```
 bool earliestChanged = insert(timer);//该timer, s是否最早到时
    if(earliestChanged ){//为什么最早到时是需要reset
        resetTimerFd(timerFd_, timer->getExpiration());
    }
```
对于变量earliestChanged, 似乎是检查最早到时的(如果没理解错), 但是为什么最早到时需要reset(reset封装了timer_setting)

####任务进度
基本完成TimeQueue的单线程设计, 不过还没加上测试函数, 同时相应理解方面也只是表面, 


####明天任务
1. 完成测试函数
2. 了解一下对于每个Timer的删除时机, 是在handle完read的时候?


###Date-11 2017. 6. 23
发现[muduo网络库编程教程](https://github.com/chenshuo/recipes/tree/master/reactor)里有相应教程(发现恨晚...)
####当天任务进展
完成timerQueue的测试, 对于之前的疑问, 似乎能连结在一起想通, 对于timerQueue, 无非就是每次将一个functor与一个timer绑定在一起, 加入到timerSet中, EventLoop只负责执行相应Queue的addTimer, handlerRead, 同时, 由于timerfd是可以由epoll返回的, 当epoll返回时, 会自动执行相应根据timer创建好的event, (相当于把timer就当做成一个event), 时间超时会, 会自动自行相应注册好的回调时间(handlerRead), handlerRead会查找set中超时集合, 并调用集合中每个元素的run方法(注册到timer的回调函数)

####疑问点
在TimerQueue之中有个问题一直每台搞懂, 下面这个函数是做莫子用的.. 首先他最对读取8byte, 另外它读取到的会是什么
````
void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
  if (n != sizeof howmany)
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}
````
查找了一下发现这个函数会在handleRead中调用, handleRead里面传入给该函数的timerFd中的timerfd_变量, 翻阅了一下, 这根timerfd的read有关, timerfd的read返回的是超时次数
当定时器超时，read读事件发生即可读，返回超时次数（从上次调用timerfd_settime()启动开始或上次read成功读取开始），它是一个8字节的unit64_t类型整数，如果定时器没有发生超时事件，则read将阻塞若timerfd为阻塞模式，否则返回EAGAIN 错误（O_NONBLOCK模式），如果read时提供的缓冲区小于8字节将以EINVAL错误返回。


####下次工作预计


###Date-12 2017. 6. 24
今天忙完了ssh后, 没有太多时间继续搞, 所以随便温习一下, 同时看下下一步的实现

####下次工作预估
这次给EventLoop增加的内容, 核心无非就是pendingFunctor, 这个东西避免线程间的干扰吧. 其实补太理解具体的用意. 不过之中用到一个pipe fd唤醒线程, 这个地方之前阿里面试官问过我, 感觉这里可以认真了解一下.

