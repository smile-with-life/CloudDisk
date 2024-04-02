#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTimer>

class Network : public QObject
{
    Q_OBJECT
private:
    bool m_OnlineState=false;//网络状态
    QTimer *p_onlinetimer=nullptr;
public:
    explicit Network(QObject *parent = nullptr);

    ~Network();

    static Network& getInstance();//获取单例模式

    bool getOnlineState();//获取网络连接状态
private:
    void Init();

    void ConnectNetwork();//检查网络连接
signals:
    void network_connected();//网络已连接信号

    void network_disconnect();//网络断开连接信号
private slots:

};

#endif // NETWORK_H
