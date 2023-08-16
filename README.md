# 项目概述
### 项目文件结构
``` DIR
|--    cpp-tools-box
    |-- CMakeLists.txt
    |-- LICENSE
    |-- README.md
    |-- bin
    |-- build   
    |-- lib
    |-- thirdparty
    |   |-- include
    |   |-- lib
    |-- src
    |   |-- main.cpp   
    |   |-- app         
    |   |-- common   
    |   |-- driver
    |   |-- db
    |   |-- event
    |   |-- net
    |   |-- config
```

## 项目介绍
### 系统架构
项目 参考很多开源项目,做到了整体解耦 
除了必要的base模块和event模块其他可以通过CMake选项增删
<img src="res/sys_architeture.png" alt="System Architecture New" height="450">


 上图是整个系统架构的一个简单概括性图示.
### 项目构建

使用**CMake**构建本项目.
当您在项目的根路径时, 可以执行以下指令:

```console
$ cd Suzuha
$ mkdir build && cd build
$ cmake .. 
$ make
将在 bin 目录生成默认应用程序 
```
### 用例展示

#### 日志模块
宏定义有四个级别的日志记录:
+ `CLOG_DEBUG`
+ `CLOG_INFO`
+ `CLOG_WARN`
+ `CLOG_ERROR`

本模块提供两种风格的API日志输出方式
创建日志流匿名类的方式实例化日志流对象，在其析构时输出到控制台 并将日志内容输入到文件,日志模块格式如下
```CPP

CLOG_INFO() << "This is C++ Style INFO Log";
CLOG_INFO_FMT("%s","This is C Style INFO Log");

//Terminal
[1970.01.01-09:49:56][ INFO ][function][line] This is C++ Style INFO Log
[1970.01.01-09:49:56][ INFO ][function][line] This is C Style INFO Log

```
#### Eventloop(事件循环池)
底层使用IO多路复用epoll 传统**Reactor模型多线程响应式架构**
内部通过事件回调 执行相关任务配合ThreadPool可以进行大量计算和阻塞操作
```CPP
//向标准输入流 注册一个FD读事件
EpollLoop loop = Loop::New()
auto fds = loop->creatFdEvent("STDIO");
//初始化
fds->initialize(STDIN_FILENO,1,Event::Mode::kPersist);
//注册回调
fds->setReadCallback([&](int fd){
    char buffer[256];
    bzero(buffer, sizeof(buffer));
    read(STDIN_FILENO, buffer, sizeof(buffer));
    std::cout << buffer << std::endl;
});
//使能读取
fds->enableReading();
//开启事件循环等待相应
loop.runloop();
```
#### Device(外设模块)
```CPP
//创建一个串口模块 
uart1 = IODevice::creatSerialDevice(loop_,"UART1");

//初始化串口
if(uart1->init("/dev/ttyS4")) 
{
    //设置接收回调
    uart1->setReadCallback([](const char *data, int dataLen){
            ByteBuf buf(data,dataLen);
            std::cout << buf.data() << "  "<< dataLen  <<std::endl;
            //将接收的数据写会串口
            uart1->send(buf.data(),dataLen);
    });
}
uart1->start();
```
#### DataBase(数据库)
暂时只封装了Sqlite

####Broker(应用程序内部消息分发)



####Loop提交事件












