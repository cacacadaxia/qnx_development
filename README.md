### QNX开发笔记

[TOC]

#### 资料来源

本项目的资料均来自于QNX的官方IDE（QNX Momentics IDE）的帮助文档：**Getting  Started with QNX Neutrino: A Guide for Realtime Programmers**。加深对于系统的库函数进行整理与理解，与对RTOS的认识，从这本书开始是极为方便和快捷的。

另外，相对应的CSDN[博客](https://mp.csdn.net/console/article)提供了重要章节的翻译，阅读起来会相对便利一些。[此外知乎](https://zhuanlan.zhihu.com/p/151033583)博主的工作也较好的参考价值，对于IPC[的理解也参考这位博主](https://www.jianshu.com/p/2be8a12f8cce)。

#### 代码运行方式

1. 每个文件夹代表一个进程，导入到IDE中去

![image-20200727150606480](https://chendaxiashizhu-1259416116.cos.ap-beijing.myqcloud.com/image-20200727150606480.png)

2. 编译build

![image-20200727150659835](https://chendaxiashizhu-1259416116.cos.ap-beijing.myqcloud.com/image-20200727150659835.png)

3. 运行

![image-20200727150730121](https://chendaxiashizhu-1259416116.cos.ap-beijing.myqcloud.com/image-20200727150730121.png)

#### 代码结构

项目主要讨论服务器与客户端之间的复杂通信关系，因此一般有对应的两个项目进行数据的收发工作。需要将对应的两个进程同时运行。

#### 联系我

csdn [blog](https://mp.csdn.net/console/article)。





