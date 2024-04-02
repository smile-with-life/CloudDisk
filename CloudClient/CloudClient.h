#ifndef CLOUDCLIENT_H
#define CLOUDCLIENT_H

#include "Forget.h"
#include "Network.h"
#include <QWidget>
#include <QTcpSocket>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui{
class CloudClient;
}
class Work;
class Logon;
class Information;
class Signup;
QT_END_NAMESPACE

class CloudClient : public QWidget
{
    Q_OBJECT
private:
    Work *sp_Worker=nullptr;//工作对象
    Network *sp_Network=nullptr;//网络状态对象
    Information *sp_Info=nullptr;//信息对象

    Logon *p_Logon=nullptr;//登录模块
    Signup *p_Signup=nullptr;//注册模块
    Forget *p_Forget=nullptr;//找回模块
public:
    explicit CloudClient(QWidget *parent = nullptr);//构造函数

    ~CloudClient();//析构函数
private:
    void Init();//初始化
signals:
    void connect_server();//连接服务器请求信号
public slots:
    void Network_Connected();//网络连接处理

    void Network_DisConnect();//网络未连接处理

    void Server_Connected();//连接成功处理

    void Server_DisConnect();//连接断开处理

    void Logoning();//登录处理
private slots:
    void on_comboBox_account_activated(int index);//下拉框选中槽函数

    void on_comboBox_account_editTextChanged();//下拉编辑框改变槽函数

    void on_pushButton_logon_clicked();//登录按钮槽函数

    void on_checkBox_visible_password_stateChanged(int arg1);//可见框槽函数

    void on_pushButton_forgot_clicked();//忘记密码按钮(槽函数)

    void on_pushButton_signup_clicked();//注册按钮槽函数
private:
    Ui::CloudClient *ui;
};


#endif // CLOUDCLIENT_H
