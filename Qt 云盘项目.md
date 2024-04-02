

# 云盘--面向个人的远程存储服务

## 项目描述

这是一份用于学习Qt编程的实战项目

使用Qt C++开发了一款面向个人的私有网盘,支持登录,注册,找回操作,支持对文件的各种操作,如新建文件夹,修改文件名,删除文件,上传和下载文件等,同时使用多线程并发编程,实现了同时对多个文件进行上传和下载操作.

## 涉及的知识点

qt 常用的界面设计组件的使用,qt 多窗体的设计,文件读写,数据库,多线程,TCP / IP网络编程,并发程序设计(qt 线程池)

## 运行截图

![image-20240402121555900](D:\Note\图片\云盘项目\image-20240402121555900.png)

![image-20240402121619370](D:\Note\图片\云盘项目\image-20240402121619370.png)

![image-20240402121647262](D:\Note\图片\云盘项目\image-20240402121647262.png)

## 开发环境及工具

处理器:AMD Ryzen 7 5800H with Radeon Graphics

操作系统:Windows 11 家庭中文版

(1)Qt Creator 12.0.0

构建系统 qmake

编译套件:

编译器:MinGW 11.2.0 64-bit

源码编码格式: UTF-8

调试器:Auto-detected CDB

Qt版本:Qt6.3.2(mingw_64)

(2)数据库Sqlite3

## 系统架构

![image-20240402122433407](D:\Note\图片\云盘项目\image-20240402122433407.png)

## 客户端设计

![image-20240402122541790](D:\Note\图片\云盘项目\image-20240402122541790.png)

## 服务器设计

![image-20240402122706263](D:\Note\图片\云盘项目\image-20240402122706263.png)

## 数据库设计

用户表(UserInfo)

 验证码表(CodeInfo)

| 用户名   | 密码     | 登录状态 | 注册时间  |
| -------- | -------- | -------- | --------- |
| Username | Password | Online   | StartTime |

| 手机号      | 验证码 | 有效时间 |
| ----------- | ------ | -------- |
| PhoneNumber | Code   | Duration |

## 通信协议设计

```c++
struct TCP_PDU//传输协议
{
    unsigned int TCP_PDULength;//协议数据单元大小
    unsigned int MessageType;//消息类型
    unsigned int Status;//状态码
    unsigned int MessageLength;//实际消息长度
    char Data[128];//定长数据
    int Message[];//实际消息（不定长数据）
};
```

```
enum MESSAGE_TYPE{...};//消息类型的枚举
enum FILETYPE{...}//文件类型的枚举
enum STATUS{...}//状态码的枚举
struct FILEINFO//文件结构
{
    char FileName[64];//文件名字
    unsigned int FileType;//文件类型
     unsigned long long FileSize;//文件大小
}
```

## 程序控制流程

ui界面与工作模块之间进行数据交换

![image-20240402140002784](D:\Note\图片\云盘项目\image-20240402140002784.png)

1.在程序开始时,work类会初始化一个工作线程,并将用于维护日常通信的Task类移入该线程中

2.当收到上传文件信号时,work类会查询线程列表,若线程列表未满,则初始化一个工作线程,将Send类移入,处理文件发送操作

3.服务器有三个继承QTcpServer的Server类用分别处理常规用户请求,文件上传和文件下载

4.服务器使用Qt自带的QThreadPool处理耗时的I/O操作