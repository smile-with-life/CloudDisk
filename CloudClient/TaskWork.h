#ifndef TASKWORK_H
#define TASKWORK_H

#include <QObject>
#include <QTcpSocket>
#include "protocol.h"


class TaskWork: public QObject
{
    Q_OBJECT
private:
private:
    QString m_IP;//IP地址
    quint16 m_TaskPort;//Task端口号
    QTcpSocket* p_TaskSocket=nullptr;
    bool m_TaskConnectFlag=false;
    bool m_IdleState=false;
public:
    explicit TaskWork(QObject *parent = nullptr);//构造函数

    ~TaskWork();//析构函数

    void Init();//初始化

    void LoadConfig();//加载配置文件


signals:
    void server_connected(QObject *myWork);//服务器连接成功信号

    void server_disconnect(QObject *myWork);//服务器断开连接信号

    void logon_respond(unsigned int Status,QString UserName,QString PhoneNumber);//登录回复信号

    void getcode_respond(unsigned int Status);//获取验证码回复信号

    void signup_respond(unsigned int Status);//注册回复信号

    void forget_respond(unsigned int Status);

    void currentpath_respond(unsigned int Status,FILEINFO *FileList,int filecount);//查询当前路径下文件回复信号

    void built_dir_respond(unsigned int Status);//新建文件夹回复信号

    void rename_file_respond(unsigned int Status);//重命名文件回复信号

    void delete_file_respond(unsigned int Status);//删除文件回复信号

    void copymove_respond(unsigned int Status);//复制移动回复信号

    void cutmove_respond(unsigned int Status);//剪切移动回复信号
public slots:
    void ConnectServer();//连接服务器

    void Send_Logon_Request(const QString Account,const QString Password);//发送登录请求

    void Send_Getcode_Request(const QString PhoneNumber);//发送获取验证码请求

    void Send_Signup_Request(const QString UserName,const QString Password,const QString PhoneNumber,const QString Code);//发送注册请求

    void Send_CurrentPath_Request(const QString CurrentPath);//发送查询当前路径下文件请求

    void Send_Built_Dir_Request(const QString CurrentPath,const QString Dirname);//发送新建文件夹请求

    void Send_CopyMove_Request(const QString SourcePath,const QString DestPath);//文件复制移动请求

    void Send_CutMove_Request(const QString SourcePath,const QString DestPath);//文件剪切移动请求

    void Send_Rename_Request(const QString CurrentPath,const QString Filename,const QString Newname);//文件重命名请求

    void Send_Delete_Request(const QString CurrentPath,const QString Filename);//文件删除请求


private:
    void recvMsg();//接收信息

    void Server_Online();

    void Server_Offline();

};

#endif // TASKWORK_H
