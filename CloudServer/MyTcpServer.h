#ifndef MyTcpServer_H
#define MyTcpServer_H

#include <QTcpServer>
#include <QList>
#include <Qthread>
enum SERVER_TYPE
{
    SERVER_TYPE_NULL=0,
    SERVER_TYPE_TASK,
    SERVER_TYPE_SEND,
    SERVER_TYPE_RECEIVE,
    SERVER_TYPE_MAX=0xffffff
};

class MyTcpSocket;
class MyTcpServer : public QTcpServer
{
    Q_OBJECT
private:
    QList<QTcpSocket*>m_TcpList;//TCP连接表
    QList<QThread*>m_ThreadList;//线程连接表
    unsigned int m_ServerType=0;
signals:
    void SendStart(qintptr socketDescriptor);
public:
    explicit MyTcpServer(unsigned int ServerType,QObject *parent = nullptr);//构造函数

    ~MyTcpServer();//析构函数

    void incomingConnection(qintptr socketDescriptor);//有TCP连接过来自动调用该函数

public slots:
    void deleteSocket(QTcpSocket *mysocket);//删除TCP连接表中的断开连接的TcpSocket



};

#endif // MyTcpServer_H
