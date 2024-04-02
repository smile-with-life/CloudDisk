#include "Logon.h"
#include "ui_Logon.h"

#include <QMessageBox>
#include <QMovie>
#include "protocol.h"
Logon::Logon(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Logon)
{
    ui->setupUi(this);
    qDebug()<<"Logon built";
    Init();//初始化

}

Logon::~Logon()
{
    delete ui;
    if(p_movie!=nullptr)
        p_movie->deleteLater();
    if(p_logontimer!=nullptr)
        p_logontimer->deleteLater();
    qDebug()<<"Logon delete";

}

void Logon::Logonshow(QString account, QString password)//进行登录
{
    this->show();
    m_LogonState=true;
    //播放动画
    p_movie=new QMovie(":/images/Book.gif");
    p_movie->setScaledSize(QSize(100,100));
    ui->label_logon_loading->setMovie(p_movie);
    p_movie->start();

    p_logontimer=new QTimer(this);//创建一个定时发送登录信号的定时器
    p_logontimer->start(2000);//定时两秒
    connect(p_logontimer, &QTimer::timeout, this,[=]()
    {
        p_logontimer->deleteLater();
        p_logontimer=nullptr;
        if(m_LogonState)
        {
            //qDebug()<<"logon_request";
            emit logon_request(account,password);
        }
    });

}

void Logon::Logonclose()
{
    m_LogonState=false;
    if(p_movie!=nullptr)
    {
        //qDebug()<<"movie delete";
        p_movie->deleteLater();
        p_movie=nullptr;
    }
    if(p_logontimer!=nullptr)
    {
        //qDebug()<<"logontimer delete";
        p_logontimer->deleteLater();
        p_logontimer=nullptr;
    }
    this->hide();
}

void Logon::Init()//初始化
{
    //界面设置
    this->setWindowFlags(Qt::Dialog);//设置窗口标志
    this->setWindowFlag(Qt::FramelessWindowHint);//不显示边框
    this->setWindowModality(Qt::WindowModal);//阻塞父窗口

    //成员设置
    sp_info=&Information::getInstance();
}

void Logon::on_pushButton_cancel_clicked()//取消按钮(槽函数)
{
    Logonclose();
}

void Logon::Logon_Respond(const unsigned int status,const QString username,const QString phone_number)//登录回复处理(槽函数)
{
    switch(status)
    {
    case LOGON_TRUE:
    {
        //QMessageBox::information(this,"登录","登录成功");
        sp_info->SetUserInfo(username,phone_number);
        emit logoning();

        break;
    }
    case LOGON_FALSE_ACCOUNT:
    {
        QMessageBox::information(this,"登录","登录失败:账号或密码不正确");
        break;
    }
    case LOGON_FALSE_ONLINE:
    {
        QMessageBox::information(this,"登录","登录失败:账号已在其它地方登录");
        break;
    }
    }
    Logonclose();

}
