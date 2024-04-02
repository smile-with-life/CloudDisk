#ifndef CLOUDSERVER_H
#define CLOUDSERVER_H

#include "MyTcpServer.h"

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class CloudServer;
}
QT_END_NAMESPACE

class CloudServer : public QWidget
{
    Q_OBJECT

private:
    QString m_LocalIP;//本机IP地址
    quint16 m_TaskPort;//Task端口号
    quint16 m_SendPort;//Send端口号
    quint16 m_ReceivePort;//Receive端口号
    MyTcpServer *p_TaskTcpServer=nullptr;//Task服务器
    MyTcpServer *p_SendTcpServer=nullptr;//Send服务器
    MyTcpServer *p_ReceiveTcpServer=nullptr;//Receive服务器

public:
    CloudServer(QWidget *parent = nullptr);

    ~CloudServer();

    void Init();//初始化

    void getLocalIP();//获取本机IP

private slots:


private:
    Ui::CloudServer *ui;
};
#endif // CLOUDSERVER_H
