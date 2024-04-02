#include "MyTcpServer.h"
#include "MyTcpSocket.h"
#include "ReceiveSocket.h"
#include "SendWork.h"

#include <QThread>


MyTcpServer::MyTcpServer(unsigned int ServerType,QObject *parent)//构造函数
    : QTcpServer{parent}
    , m_ServerType(ServerType)
{

}

MyTcpServer::~MyTcpServer()
{
    //删除m_TcpList表
    QList<QTcpSocket*>::iterator socketiter=m_TcpList.begin();
    for(;socketiter!=m_TcpList.end();)
    {

        if((*socketiter)!=nullptr)
        {
            (*socketiter)->deleteLater();
            *socketiter=nullptr;
            m_TcpList.erase(socketiter);
        }
        else
        {
            socketiter++;
        }
    }
    //删除m_TcpList表
    QList<QThread*>::iterator threaditer=m_ThreadList.begin();
    for(;threaditer!=m_ThreadList.end();)
    {

        if((*threaditer)!=nullptr)
        {
            (*threaditer)->quit();
            (*threaditer)->wait();
            (*threaditer)->deleteLater();
            *threaditer=nullptr;
            m_ThreadList.erase(threaditer);
        }
        else
        {
            threaditer++;
        }
    }

}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)//有TCP连接过来自动调用该函数
{
    switch(m_ServerType)
    {
    case SERVER_TYPE_NULL:
    {
        qDebug()<<"服务器类型未设置!";
        exit(0);
        break;
    }
    case SERVER_TYPE_TASK:
    {
        MyTcpSocket *pMyTcpSocket =new MyTcpSocket;//申请一个本机套接字
        qDebug()<<"new tcp connect Task "<<pMyTcpSocket;
        pMyTcpSocket->setSocketDescriptor(socketDescriptor);//设置本机套接字描述符
        m_TcpList.append(pMyTcpSocket);//把套接字描述符放入TCP连接表中
        connect(pMyTcpSocket,&MyTcpSocket::offline,this,&MyTcpServer::deleteSocket);
        break;
    }
    case SERVER_TYPE_SEND:
    {
        qDebug()<<"发送线程创建";
        QThread *sendThread=new QThread;
        qDebug()<<sendThread;
        m_ThreadList.append(sendThread);
        SendWork *Work=new SendWork;
        Work->moveToThread(sendThread);
        sendThread->start();
        connect(this,&MyTcpServer::SendStart,Work,&SendWork::Start);
        connect(Work,&SendWork::finish,sendThread,&QThread::quit);
        connect(sendThread,&QThread::finished,sendThread,&QThread::deleteLater);
        connect(sendThread,&QThread::finished,this,[=](){qDebug()<<"发送线程销毁";});
        emit SendStart(socketDescriptor);
        disconnect(this,&MyTcpServer::SendStart,0,0);
        break;
    }
    case SERVER_TYPE_RECEIVE:
    {
        ReceiveSocket *pReceiveSocket =new ReceiveSocket;//申请一个本机套接字
        qDebug()<<"new tcp connect Send"<<pReceiveSocket ;
        pReceiveSocket->setSocketDescriptor(socketDescriptor);//设置本机套接字描述符
        m_TcpList.append(pReceiveSocket);//把套接字描述符放入TCP连接表中
        connect(pReceiveSocket,&ReceiveSocket::offline,this,&MyTcpServer::deleteSocket);
        break;
    }
    }
}

void MyTcpServer::deleteSocket(QTcpSocket *socket)//删除TCP连接表中的断开连接的TcpSocket(槽函数)
{
    QList<QTcpSocket*>::iterator iter;
    for(iter=m_TcpList.begin();iter!=m_TcpList.end();)
    {
        if(socket==*iter)
        {
            (*iter)->deleteLater();
            *iter=nullptr;
            m_TcpList.erase(iter);
            qDebug()<<socket<<"删除成功";
            break;
        }
        else
        {
            iter++;
        }
    }
}
