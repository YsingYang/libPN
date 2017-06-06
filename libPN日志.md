### Data_1  2017 - 6 -6
#### 想写一个网络库的缘由
   面试了TX, Ali后发现自己网络, OS方面还有点欠缺, 于是想和同学写一个小型IM系统来弥补一下自己的缺陷..可是发现自己写的网络模型有点糟糕...写到后面不知道从何下手, 虽说基本上能完成接收发送数据了, 足够实验室用了, 但是感觉自己并没有完全了解到什么, 之前知乎上看到一些大神推荐看看nginx, libevent, redis的源码, 刚好Ali面试后面试官也推荐我看看libevent, redis的源码, libevent也算是属于我现在想写的reactor模型的典范, 于是想参照相应的rector模型, 如muduo, libevent, 来写一个网络库
   
#### 问题
  参考别人的源码中发现agiNet中的EventLoop 好像拆分成了一个EventLoop与EventCtrl, 我觉得有好有不好, 这多一层封装上的意义其实不太大, 故跟muduo一样直接写成一个EventLoop里, 不知agiNet作者是否别有用意, 还是我理解有误
  

#### 进展
  基本完成EventLoop思路
   