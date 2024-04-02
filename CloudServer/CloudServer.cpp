#include "CloudServer.h"
#include "ui_CloudServer.h"

#include<QHostInfo>
#include<QList>
#include<QMessageBox>
#include<MyTcpServer.h>
#include <QThreadPool>

CloudServer::CloudServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CloudServer)
{
    ui->setupUi(this);
    Init();
}

CloudServer::~CloudServer()
{
    delete ui;
    if(p_TaskTcpServer!=nullptr)
        p_TaskTcpServer->deleteLater();
    if(p_SendTcpServer!=nullptr)
        p_SendTcpServer->deleteLater();
    if(p_ReceiveTcpServer!=nullptr)
        p_ReceiveTcpServer->deleteLater();
}

void CloudServer::Init()//初始化
{
    getLocalIP();
    qDebug()<<m_LocalIP;
    //采用本地连接
    m_LocalIP="127.0.0.1";
    m_TaskPort=7777;
    m_SendPort=9999;
    m_ReceivePort=8888;
    p_TaskTcpServer=new MyTcpServer(SERVER_TYPE_TASK);
    p_SendTcpServer=new MyTcpServer(SERVER_TYPE_SEND);
    p_ReceiveTcpServer=new MyTcpServer(SERVER_TYPE_RECEIVE);
    p_TaskTcpServer->listen(QHostAddress(m_LocalIP),m_TaskPort);//Task服务器开始监听
    p_SendTcpServer->listen(QHostAddress(m_LocalIP),m_SendPort);//Send服务器开始监听
    p_ReceiveTcpServer->listen(QHostAddress(m_LocalIP),m_ReceivePort);//Receive服务器开始监听
    QThreadPool::globalInstance()->setMaxThreadCount(4);// 设置线程池的最大线程数
}

void CloudServer::getLocalIP()//获取本机IP
{
    m_LocalIP="";
    QString hostName=QHostInfo::localHostName();//计算机主机名
    QHostInfo hostInfo=QHostInfo::fromName(hostName);//返回的QHostInfo对象包含已解析的域名和主机的IP地址
    QList<QHostAddress> hostIplist=hostInfo.addresses();//获取本机IP地址列表
    if(!hostIplist.isEmpty())
    {
        for(int i=0;i<hostIplist.count();i++)
        {
            QHostAddress host=hostIplist.at(i);
            if(QAbstractSocket::IPv4Protocol==host.protocol())//获取本机IPv4地址
            {
                m_LocalIP=host.toString();
                break;
            }
        }
    }
    else
    {
        QMessageBox::critical(this,"获取本机IP地址","获取本机IP地址失败");
    }
}

