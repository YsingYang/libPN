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

