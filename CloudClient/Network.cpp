#include "Network.h"

#include <QLibrary>
#include <QMessageBox>

Network::Network(QObject *parent)
    : QObject{parent}
{
    Init();
}

Network::~Network()
{
    if(p_onlinetimer!=nullptr)
        p_onlinetimer->deleteLater();

}

void Network::Init()
{
    ConnectNetwork();
    p_onlinetimer = new QTimer(this);//创建检查网络连接的定时器
    connect(p_onlinetimer, &QTimer::timeout, this,[=](){ConnectNetwork();p_onlinetimer->start(500);});
    p_onlinetimer->start(500);//启动定时器(定时500ms)
}

Network &Network::getInstance()
{
    static Network instance;
    return instance;

}

void Network::ConnectNetwork()
{
    typedef bool(*ConnectFun)(int* lpdwFlags, int  dwReserved);
    QLibrary lib("Wininet.dll");
    //如果正确加载了dll
    if (lib.load())
    {
        int  flags;
        //获取dll库中的函数InternetGetConnectedState函数地址`
        ConnectFun myConnectFun = (ConnectFun)lib.resolve("InternetGetConnectedState");
        //判断是否连网
        m_OnlineState = myConnectFun(&flags, 0);
        if (m_OnlineState)
        {
            //qDebug()<<"已连接网络";
            emit network_connected();
            //ui->label_network_state->setText("");
        }
        else
        {
            //qDebug()<<"网络连接断开";
            emit network_disconnect();
            //ui->label_network_state->setStyleSheet("color:red;");
            //ui->label_network_state->setText("网络连接超时");
        }
    }
    else
    {
        QMessageBox::warning(nullptr,"系统错误","计算机中丢失wininet.dll文件，尝试安装该程序以解决此问题");
    }

}

bool Network::getOnlineState()
{
    return m_OnlineState;
}
