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


### Date-13 2017. 6. 25
昨天对EventLoop增加了一些成员变量处理pendingFunctor, 之中涉及到一个wakeUpfd, 用于唤醒线程处理相应事件, 对于该fd的创建, 用到了一个createfd函数
```
int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
这个函数会创建一个 事件对象 (eventfd object), ** 用来实现，进程(线程)间的等待/通知(wait/notify) 机制 **. 内核会为这个对象维护一个64位的计数器(uint64_t)。
```
man page原话
> The object contains an unsigned 64-bit integer (uint64_t) counter that is maintained by the kernel.  This counter is initialized with the value specified in the argument initval.



### Date-14 2017. 6. 26
####内容 : 完成runInLoop的设计
之前一致想补明白的是, handlerRead, 与wakeup到底什么卵用, 不就是一个写一个读吗, 如果没有, epoll_Loop还是继续执行, doPendingFunctor函数也会继续执行, 好像可有可无一样, 仔细想想其实不是的, 因为我忘记了epoll也是会阻塞在epoll_wait上的!
> 
解释一下这里唤醒的意思, 刚才拍了下脑袋竟然想明白了
我们知道如果对epoll进行定时,(或者说无限等待-1), 那么线程将会阻塞在epoll_wait()上
这时候为了唤醒该线程, 我们需要人为地给该线程制造一个事件, 而这里用到的就是wakeupFD,

另外一点, 关于pendingFunctor这个数组, 由于前提reactor模式下是one loop one thread的, 所以说, 可能会有其他线程执行并不属于他自己的EventLoop, 但是pengdingFunctor一定只属于某个线程的, 所以别的线程只会将cb放入pendingFunctor中然后wakeup一下, 就走人, 执行pendingFunctor的一定是所属线程他本身

####测试
通过测试函数, 当给不同的线程调用runInLoop的时候, 其他线程只会将callback放入pendingFunctors中

### Date-15 2017. 6. 27
####内容 : 提高TimeQueue的线程安全性
1. 首先实验一段muduo中crash的代码, 但是我这边并没有crash, 而是停在那里. 不过后来想了下..因为我没有调用QUIT. 后来发现, 原来我已经按照源代码将这个问题解决了..这里就牵扯到了addTimerInLoop这个函数的作用, 它是用来保证线程安全的
   我们知道runInLoop这个函数是线程安全的, 如果我们将addTimerInLoop抽象出来, 作为一个回调, 交给runInLoop来处理的话, 就可以保证添加Timer的线程安全了
   
2. 添加EventLoopThread class
 这里实现要跟muduo有一点不同, 因为我使用的是C++11的thread类, 而muduo有自带的thread类.
 moduo, thread有自带start方法, 所以保证, 只有在startLoop之中调用thread.start的时候, threadFunc才会启动, 但是我的实现不会, 因为我是将函数直接传给thread的构造函数, 那么, 就会有可能threadFunc在start前执行, 
 
 我的想出来的处理方法有两个
 1.

 ```
 {
        std::lock_guard<std::mutex> lck(mutex_); ///为了保证threadFunc在startLoop调用后执行, 构造一个领界区, 里面有thread temp变量, swapEventLoopThread中的thread_
        std::thread temp(std::bind(&PNEventLoopThread::threadFunc, this));
        swap(temp, thread_);
    }
    ```
 2.

 我觉得只要在threadFunct中调用一个cond_.wait()
 然后再在startThread开始时添加个notify()就可以了
 
 
 现在暂时用第一种代码, 虽然按道理来说, 第二种方案会相对比较优化


### Date-16 2017. 6. 28
实现完Event那一块， 和TimerQueue那一块后。 基本底层的就实现的差不多了， 可以将这些封装了一个Server(假定mutex_这些用现有的实现)
####内容 : Acceptor的实现
在PassingNote中， 我原本想的实现是sockfd, 与sockaddr合在一起成为一个socketStruct, 但是这边的实现是抽离开，感觉分开也有一定道理， 低耦合那么有没有更好的实现
今天主要实现了inet类， 基本完成了相关实现， 不过一些封装上没有跟muduo完全一样， 主要是自己需求上吧..不过基本一样就是了。。


### Date-17 2017. 6. 29
终于考完算法了..不过感觉自己考的不理想就是了..也不能全怪昨晚没睡好， 实力问题吧



1. 今天发现了一个SOMAXCONN 定义与 <sys/socket.h> , 其实是用于listen的第二个参数（listen队列最大连接数）， 有网上说一般Linux默认值为128， 如果对于负载大的服务器来说显然太小？ 但所有服务器都是128吗？这是man page上的描述
```
If the backlog argument is greater than the value in /proc/sys/net/core/somaxconn, then it is silently truncated to that value; the default value in this file is 128. In kernels before 2.4.25, this limit was a hard coded value, 
SOMAXCONN, with the value 128.
```

2. SO_REUSEADDR
 之前没有太注意这个setsockopt中的这个Flag, 其实这个字段是用于让端口释放后立即就可以被再次使用， 一般来说，一个端口释放后会等待两分钟之后才能再被使用， 这也就是之前我测试的时候经常中断程序， 又开启的时候出现的binderror , 就是因为设置reuse, 但他其实还有其他用途， 这里抄自其他博客
 ```
     SO_REUSEADDR允许启动一个监听服务器并捆绑其众所周知端口，即使以前建立的将此端口用做他们的本地端口的连接仍存在。这通常是重启监听服务器时出现，若不设置此选项，则bind时将出错。
    SO_REUSEADDR允许在同一端口上启动同一服务器的多个实例，只要每个实例捆绑一个不同的本地IP地址即可。对于TCP，我们根本不可能启动捆绑相同IP地址和相同端口号的多个服务器。
    SO_REUSEADDR允许单个进程捆绑同一端口到多个套接口上，只要每个捆绑指定不同的本地IP地址即可。这一般不用于TCP服务器。
    SO_REUSEADDR允许完全重复的捆绑：当一个IP地址和端口绑定到某个套接口上时，还允许此IP地址和端口捆绑到另一个套接口上。一般来说，这个特性仅在支持多播的系统上才有，而且只对UDP套接口而言（TCP不支持多播）。
 ```
 
3. 返回引用值， 之前在研究项目工程上不知道如何返回一个引用， 今天惊奇的发现好像可以返回一个常量引用
```
const struct sockaddr_in& getSockAddrInet() const { return addr_; }
如果把const去掉就是错误的..很神奇..这是为了保护不返回空引用吗？
```
4. 今天在一个常出的笔试题上出BUg, 我把一个数组指针传入函数， 再对这个指针取sizeof()这时候在该函数里返回的是一个指针大小的值8， 而不是数组长度， 如果在原函数上就是数组长度


### Date-18 2017. 6. 30
开始时将muduo书中的两个扩展练习写了一下
1. 将test7.cc改写为daytime服务器, 这个就是在acceptor的回调函数中用timestamp的now().formatString即可。
2. 同时监听2个port, 看到这里的时候突然想回去看自己当初的EventLoop, event, epoll是怎么协调工作的...这里复习一下， 当我们创建Event时并没有将event加入到epoll中， 而EventLoop放置event更无从谈起， 直到event.setcallback的时候， 会调用update, update会调用loop->update, 再调用epoll->update， 这时候才是加入到epoll中
 这个问题也很简单， 多写一个Acceptor就可以了。
 
现在只是将Acceptor简单的调用一个回调。 对于一个服务器来说， 应该需要调用一个新连接函数， 而muduo中会创建TcpServer类。 现在先处理连接建立

# 学到一个新东西enable_shared_from_this
在muduo的connection类中出现了一个boost::enable_shared_from_this<T> 这个奇奇怪怪的东东， 这个东西是为了让类自身能获取到自身的智能指针， 比如我用shared_ptr<T>封装了一个T, 当我们在T类中， 得到的仅仅是一个this裸指针， 我们可以通过继承上面那个奇奇怪怪的东东得到获取自身智能指针。
**为什么要有这么奇葩的东西**
知乎上轮子哥给出了他的回答
> 然而在类的内部获得自己的shared_ptr这件事情是很难避免的

**也有人会问 : 我直接写个函数， 把this包装一下返回一个shared_ptr不就行了嘛**
这样子做返回的并不是该this的shared_ptr， 不巧它重新创建了一个新的shared_ptr, 相当于有两个不同的shared_ptr指向一个实例。 
[Cplusplus](http://en.cppreference.com/w/cpp/memory/enable_shared_from_this) CPPreference给出了一个example.
至于原理， 在[blog](http://blog.guorongfei.com/2017/01/25/enbale-shared-from-this-implementaion/)中有说明， 本质上是会shared_ptr这个类做了点文章， 自己维护一个哈希， 或者成员变量， 当调用get返回就行了
```
class shared_ptr<Widget> {
public:
    explicit shared_ptr<Widget>(Widget* w) {
        // 其他初始化，可能在初始化列表中而不是构造函数内部
        RegisterSharedPtr(w, this);
    }
};
void RegisterSharedPtr(Widget* w, shared_ptr<Widget>* sp) {
    // 建立 w 到 sp 的映射关系
}
void GetSharedPtr(Widget* w) {
    // 查找 w 对应的 sp
}
class Widget {
public:
    shared_ptr<Widget> shared_from_this() {
        GetSharedPtr(this);
    }
};
```


### Date-19 2017. 7. 1
继续完成昨天的任务， 昨天只完成了Connection.h的实现. 
1. 在TCPServer中发现了一个成员变量ConnID,从字面意义上理解， 似乎是用于记录连接上服务器的后给这些客户端分配的一个id,还能起到统计数量的作用， 但实际有没有更深的意义或者说别的意义还有待考察
2. 还有第二个问题是，在TcpServer::newConnection函数中， 传入的sockfd函数， 到底是监听的fd, 还是在accept后得到的fd, ?????
3. 在C++11中， 有时候我们设置回调时用到bind, 但同时也会用到占位符号， 一般网上都是直接_1， _2, 但这样一般会报编译错误， C++11提供了std::placeholders::_x..来替代占位符

* 回到问题2上 *
问题2中的调用是， accept的一个回调函数中调用的， 即acceptor的Event被触发了， 于是调用了acceptor的callback, 这时的callback()传入的参数， 正是建立连接（accept后的fd）
我提出这个问题主要还是对getsockname这个函数的不太理解， 网上给出的解释是
```
getsockname函数返回与套接口关联的本地协议地址。
getpeername函数返回与套接口关联的远程协议地址。
```

在不调用bind的TCP客户，当connect成功返回后，getsockname返回分配给此连接的本地IP地址和本地端口号；
在捆绑了通配IP地址的TCP服务器上，当连接建立后，可以使用getsockname获得分配给此连接的本地IP地址；
当一个服务器调用exec启动后，他获得客户身份的唯一途径是调用getpeername函数。

简单理解的话， 我觉得应该是当得到一个新的连接fd后， 我们可以通过getsockname(fd, xx, xx)或者与该sockfd绑定的本地结构地址

* 再则回到问题1 *、
其实正如我的猜测一样， connId 用在了newConnection这个处理新的连接的函数上， 无非就是在servermp的时候创建一个新的id
但同时有个问题是， 假设一个新conn连上id = 2, 又一个连接id = 3, 这时候2断开了, 如果--3那么再一个新连接的时候重复key了吗》??


### Date-20 2017. 7. 2
在之前的TCPServer, TCPConnection完成， 是没有断开连接这一操作， 所以如果有连接进来， 而后断开， 在Server上会一直在死循环的状态.
1. 在event处理close时， 判断的条件是
```
(revent_ & POLLHUP) && !(revent_ &   POLLIN)
```
与我之前写PassingNote时候有点不太一样 我之前判断的好像是revent == POLLIN, 然后read == 0 是close..
网上对POLLHUP描述是这样的
**POLLHUP, on the other hand, indicates that your file descriptor is valid, but that it's in a state where:**
> A device has been disconnected, or a pipe or FIFO has been closed by the last process that had it open for writing. Once set, the hangup state of a FIFO shall persist until some process opens the FIFO for writing or until all read-only file descriptors for the FIFO are closed. This event and POLLOUT are mutually-exclusive; a stream can never be writable if a hangup has occurred. However, this event and POLLIN, POLLRDNORM, POLLRDBAND, or POLLPRI are not mutually-exclusive. This flag is only valid in the revents bitmask; it shall be ignored in the events member.

 **POSIX中的描述**
The device has been disconnected. This event and POLLOUT are mutually-exclusive; a stream can never be writable if a hangup has occurred. However, this event and POLLIN, POLLRDNORM, POLLRDBAND, or POLLPRI are not mutually-exclusive. This flag is only valid in the revents bitmask; it shall be ignored in the events member.

2. 在TCPConnection中， 有个connectDestroyed函数， 是Connection用于主动断开连接的吗？ 不是， 这是connection存货前最后一个函数， 可以理解为处理后事

3. 在buff读取的时候， 出现了相应的错误， 不知道为什么会读取到之前的大小的字符串

### Date-21 2017. 7. 3
明天就要上班了！， 好怂啊..

1. 对function MessageCallback的修改， 现在的MessageCallback该为
```
typedef boost::function<void (const TcpConnectionPtr&, Buffer* buf, Timestamp)> MessageCallback;
```

##下次内容， 添加inputbuffer成员进 connection类中
