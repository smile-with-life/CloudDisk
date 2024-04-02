#ifndef RECEIVESOCKET_H
#define RECEIVESOCKET_H

#include <QTcpSocket>

class ReceiveSocket : public QTcpSocket
{
    Q_OBJECT
private:
    bool m_UpLoadFlag=false;
    QString m_UploadPath;
    qint64 m_Filesize;
    qint64 m_Recvsize;
public:
    explicit ReceiveSocket(QObject *parent = nullptr);

    ~ReceiveSocket();
signals:
    void offline(QTcpSocket *mysocket);//删除服务器表中信息的信号
public slots:
    void recvMsg();//接收信息
};

#endif // RECEIVESOCKET_H
