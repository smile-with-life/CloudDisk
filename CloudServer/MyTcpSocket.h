#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
private:
    QString m_Account="";
public:
    explicit MyTcpSocket(QObject *parent = nullptr);//构造函数
signals:
    void offline(QTcpSocket *mysocket);//删除服务器表中信息的信号
public slots:
    void recvMsg();//接收信息

    void Client_Offline();//客户端断开连接后的处理函数

    void Send_PhoneCode(QString PhoneNumber,QString Content);//发送手机验证码

    QString GetContent();//获得验证码

    bool CutMove(QString SourcePath, QString DestPath);

    void CopyDir(QString SourceDirPath,QString DestDirPath);

private:


};

#endif // MYTCPSOCKET_H
