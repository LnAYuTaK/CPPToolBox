# 项目概述
### 项目文件结构
``` DIR
|-- GKZD
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
项目做到了整体解耦 除了必要的base模块和event模块其他可以通过CMake选项增删
<img src="res/sys_architeture.png" alt="System Architecture New" height="450">


 上图是整个系统架构的一个简单概括性图示.
### 项目构建

使用**CMake**构建本项目.
当您在项目的根路径时, 可以执行以下指令:

```console
$ cd GWZD 
$ mkdir build && cd build
$ cmake .. 
$ make
将在 bin 目录生成默认应用程序 
```
### 用例展示

#### 日志模块
创建日志流匿名类的方式实例化日志流对象，在其析构时输出到控制台 并将日志内容输入到文件,日志模块格式如下
```C++
CLOG_INFO() << "INFO LOG"

[1970.01.01-09:49:56][ INFO ][function][line] INFO LOG

CLOG_INFO_FMT("%s","INFO LOG FMT");

[1970.01.01-09:49:56][ INFO ][function][line] INFO LOG FMT
```
宏定义有四个级别的日志记录:
+ `CLOG_DEBUG`
+ `CLOG_INFO`
+ `CLOG_WARN`
+ `CLOG_ERROR`
本模块提供两种风格的API日志输出方式
```CPP
Example: 
CLOG_INFO_FMT("%s","This is INFO Log")
```
```CPP
Example:
CLOG_INFO() << "This is INFO Log" 
```
#### Eventloop(事件循环池)
底层使用IO多路复用epoll 传统**Reactor模型多线程响应式架构**
内部通过事件回调 执行相关任务配合ThreadPool可以进行大量计算和阻塞操作
```CPP

EpollLoop loop = Loop::New()
//向标准输入流 注册一个FD读事件
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
#### 网络模块
实现基础的网络模块 TCP客户端 TCP服务器 MQTT 客户端
实现依赖底层第三方库 **MQTT** **HP-Socket** 
```CPP
实例化一个MQTT客户端
#define MQTT_URL "192.168.16.231:1883"
#define MQTT_NAME "client"
#define MQTT_USERNAME    "admin"
#define MQTT_PASSWD      "public"
auto mqttc = make_shared<MqttClient>(MQTT_URL,MQTT_NAME);
auto opts = mqtt::connect_options_builder()
                .mqtt_version(MQTTVERSION_5)
                .clean_start(true)
                .finalize();
opts.set_user_name(MQTT_USERNAME);
opts.set_password(MQTT_PASSWD);
mqttc.connect(opts);
//订阅主题
mqttc.subscribe("topic");
//发布主题
mqttc.subscribe("msg","MsgPayLoad");
```













