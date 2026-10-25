#1. 项目介绍:
   这个项目是我的一个Linux应用开发的实践项目，我在Ubuntu虚拟机h上练习使用C语言编写了一个多线程的客户端网络服务器。

#2. 功能简介：
    本项目是一个基于C语言的多线程客户端网络服务器，它能够处理多个客户端的连接和请求。服务器使用TCP协议进行通信，并实现了基本的客户端管理功能，如连接、断开连接、接收和发送消息等。

#3.技术栈：
    - C语言
    - Linux 
    - GCC 
    - TCP socket
    - pthread

#4. 项目结构：
    - README.md: 项目说明文档
    - Makefile: 编译脚本
    - server.c: 服务器端代码
    - client.c: 客户端代码
    - transmission.h: 通信头文件
    - transmission.c: 通信源文件

#5. 使用方法：
    - 编译代码：`make all`
    - 运行服务端：`./server`
    - 运行客户端：`./client`
