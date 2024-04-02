#include "CloudClient.h"
#include "ui_CloudClient.h"
#include <QMessageBox>
#include <QDebug>
#include "Signup.h"
#include "Information.h"
#include "Logon.h"
#include "OperatorWidget.h"
#include "Work.h"

CloudClient::CloudClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CloudClient)
{
    ui->setupUi(this);
    Init();//初始化
}

CloudClient::~CloudClient()
{
    delete ui;

}

void CloudClient::Init()//初始化
{
    //成员初始化
    sp_Worker=&Work::getInstance();//关联工作模块
    sp_Network=&Network::getInstance();//关联网络模块
    sp_Info=&Information::getInstance();//关联信息模块
    p_Logon=new Logon(this);//创建登录模块
    p_Signup=new Signup();//创建注册模块
    p_Forget=new Forget();//创建找回模块

    //界面设置
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);//禁止窗口最大化
    setFixedSize(this->width(),this->height());//禁止拖动窗口大小
    for(int i=0;i<sp_Info->getLogonInfoCount();i++)//设置下拉框初值
        ui->comboBox_account->addItem(sp_Info->getLogonInfoList(i).account);
    if(sp_Info->getLogonInfoList(0).password==PasswordNull)
        ui->lineEdit_password->setText("");//设置密码编辑框初值
    else
        ui->lineEdit_password->setText(sp_Info->getLogonInfoList(0).password);//设置密码编辑框初值
    ui->checkBox_auto_logon->setChecked(sp_Info->getLogonInfoList(0).auto_logon);//设置自动登录可选框初值
    ui->checkBox_remembr_password->setChecked(sp_Info->getLogonInfoList(0).remember_password);//设置记住密码可选框初值

    //信号与槽连接
    connect(this,&CloudClient::connect_server,sp_Worker,&Work::ConnectServer);

    connect(sp_Worker,&Work::server_connected,this,&CloudClient::Server_Connected);
    connect(sp_Worker,&Work::server_disconnect,this,&CloudClient::Server_DisConnect);
    connect(sp_Worker,&Work::getcode_respond,p_Signup,&Signup::Getcode_Respond);
    connect(sp_Worker,&Work::signup_respond,p_Signup,&Signup::Signup_Respond);
    connect(sp_Worker,&Work::logon_respond,p_Logon,&Logon::Logon_Respond);

    connect(p_Signup,&Signup::getcode_request,sp_Worker,&Work::Send_Getcode_Request);
    connect(p_Signup,&Signup::signup_request,sp_Worker,&Work::Send_Signup_Request);

    connect(p_Logon,&Logon::logon_request,sp_Worker,&Work::Send_Logon_Request);
    connect(p_Logon,&Logon::logoning,this,&CloudClient::Logoning);

    connect(sp_Network,&Network::network_connected,this,&CloudClient::Network_Connected);
    connect(sp_Network,&Network::network_disconnect,this,&CloudClient::Network_DisConnect);

    //连接服务器
    emit connect_server();//发送连接服务器请求信号
     qDebug()<<"线程id"<< QThread::currentThread();

}

// void CloudClient::AlterData(LOGONINFO info)//修改本地数据文件
// {
//     ACCOUNT copy[LogonInfoSize];
//     m_LogonInfoCount++;
//     //把最新登录的账户放入头节点
//     copy[0].account=info.account;
//     copy[0].password=info.password;
//     copy[0].auto_logon=info.auto_logon;
//     copy[0].remember_password=info.remember_password;
//     for(int i=1,j=0;i<LogonInfoSize && j<m_LogonInfoCount;i++)
//     {
//         //登录账户为已保存过的
//         if(info.account==m_LogonInfoList[j].account)
//         {
//             m_LogonInfoCount--;
//             j++;
//         }
//         //把以前的账户保存
//         copy[i].account=m_LogonInfoList[j].account;
//         copy[i].password=m_LogonInfoList[j].password;
//         copy[i].auto_logon=m_LogonInfoList[j].auto_logon;
//         copy[i].remember_password=m_LogonInfoList[j].remember_password;
//         j++;
//     }
//     for(int i=0;i<m_LogonInfoCount;i++)
//     {
//         m_LogonInfoList[i].account=copy[i].account;
//         qDebug()<<m_LogonInfoList[i].account;
//         m_LogonInfoList[i].password=copy[i].password;
//         qDebug()<<m_LogonInfoList[i].password;
//         m_LogonInfoList[i].auto_logon= copy[i].auto_logon;
//         qDebug()<<m_LogonInfoList[i].auto_logon;
//         m_LogonInfoList[i].remember_password=copy[i].remember_password;
//         qDebug()<<m_LogonInfoList[i].remember_password;
//     }
//     QFile file(":/LocalData.txt");
//     QByteArray Data;
//     if(file.open(QIODeviceBase::WriteOnly))
//     {
//         //QMessageBox::information(nullptr,"打开本地数据文件","打开本地数据文件成功");
//         for(int i=0;i<m_LogonInfoCount;i++)
//         {
//             QString LineData;
//             LineData.append(m_LogonInfoList[i].account);
//             LineData.append(" ");
//             LineData.append(m_LogonInfoList[i].password);
//             LineData.append(" ");
//             LineData.append(QString::number(m_LogonInfoList[i].auto_logon));
//             LineData.append(" ");
//             LineData.append(QString::number(m_LogonInfoList[i].remember_password));
//             Data.append(LineData.toStdString().c_str());
//         }
//         file.write(Data);
//         file.close();//关闭文件
//     }
//     else
//     {
//         QMessageBox::critical(nullptr,"打开本地数据文件","打开本地数据文件失败");
//     }
// }

void CloudClient::on_comboBox_account_activated(int index)//下拉框选中(槽函数)
{
    ui->comboBox_account->setCurrentText(sp_Info->getLogonInfoList(index).account);
    if(sp_Info->getLogonInfoList(index).password==PasswordNull)
        ui->lineEdit_password->setText("");
    else
        ui->lineEdit_password->setText(sp_Info->getLogonInfoList(index).password);
    ui->checkBox_auto_logon->setChecked(sp_Info->getLogonInfoList(index).auto_logon);
    ui->checkBox_remembr_password->setChecked(sp_Info->getLogonInfoList(index).remember_password);

}

void CloudClient::on_comboBox_account_editTextChanged()//下拉编辑框改变(槽函数)
{
    ui->lineEdit_password->setText("");
    ui->checkBox_auto_logon->setChecked(false);
    ui->checkBox_remembr_password->setChecked(false);
}

void CloudClient::on_checkBox_visible_password_stateChanged(int arg1)//可见框(槽函数)
{
    if(arg1)
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
    else
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);
}

void CloudClient::on_pushButton_logon_clicked()//登录按钮(槽函数)
{
    if(!p_Logon->isVisible())
    {
        QString Account = ui->comboBox_account->currentText();//从输入框中获取账户
        QString Password = ui->lineEdit_password->text();//从输入框中获取密码
        if(true)
        {
           p_Logon->Logonshow(Account,Password);//登录
        }
    }
}

void CloudClient::on_pushButton_signup_clicked()//注册按钮(槽函数)
{
    if(!p_Signup->isVisible())
    {
        p_Signup->Signupshow();//注册
    }

}

void CloudClient::on_pushButton_forgot_clicked()//忘记密码按钮(槽函数)
{

}

void CloudClient::Network_Connected()//网络连接处理
{
    ui->label_network_state->setText("");
}

void CloudClient::Network_DisConnect()//网络未连接处理
{
    ui->label_network_state->setStyleSheet("color:red;");
    ui->label_network_state->setText("网络连接超时");
}

void CloudClient::Server_Connected()//连接成功处理(槽函数)
{    
    QMessageBox::information(this,"连接服务器","连接成功");
    //自动登录功能
    if(ui->checkBox_auto_logon->isChecked()==true)
        on_pushButton_logon_clicked();
}

void CloudClient::Server_DisConnect()//连接断开处理(槽函数)
{
    QMessageBox::information(this,"连接服务器","连接断开");
}

void CloudClient::Logoning()//登录处理(槽函数)
{
    this->hide();//启动器隐藏
    OperatorWidget::getInstance().show();//主界面显示
}



