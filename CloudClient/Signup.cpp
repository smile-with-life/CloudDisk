#include "Signup.h"
#include "ui_Signup.h"
#include "protocol.h"
#include <QMessageBox>
#include <QTimer>

Signup::Signup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Signup)
{
    ui->setupUi(this);
    qDebug()<<"Signup built";
    Init();//初始化
}

Signup::~Signup()
{
    delete ui;
    qDebug()<<"Signup delete";
    if(p_timer!=nullptr)
        p_timer->deleteLater();
}

void Signup::Init()//初始化
{
    //界面设置
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);//禁止窗口最大化
    this->setFixedSize(this->width(),this->height());//禁止拖动窗口大小

}

void Signup::lock_button(int time_interval)//锁定按钮
{
    p_timer=new QTimer;
    m_time_interval=time_interval;//按钮锁定时间
    p_timer->start(1000);
    connect(p_timer,&QTimer::timeout,this,&Signup::time_show);
    ui->pushButton_getcode->setEnabled(false);
    ui->pushButton_getcode->setText(QString::number(m_time_interval));

}

void Signup::Signupshow()//显示
{
    this->show();
}

void Signup::Signupclose()//关闭
{
    this->hide();
}

void Signup::time_show()//按钮锁定时间显示(槽函数)
{
    if(m_time_interval>0)
    {
        p_timer->start(1000);
        m_time_interval--;
        ui->pushButton_getcode->setText(QString::number(m_time_interval)+"s");
    }
    else
    {
        ui->pushButton_getcode->setEnabled(true);
        ui->pushButton_getcode->setText("获取验证码");
        p_timer->deleteLater();
        p_timer=nullptr;
    }

}

void Signup::on_pushButton_getcode_clicked()//获取验证码按钮(槽函数)
{

    QString cellphonenumber = ui->lineEdit_phone_number->text();//获取手机号
    lock_button(60);//锁定按钮
    emit getcode_request(cellphonenumber);//发送获取验证码请求信号

}

void Signup::on_pushButton_signup_clicked()//注册按钮(槽函数)
{

    QString username = ui->lineEdit_username->text();//用户名
    QString password = ui->lineEdit_password->text();//密码
    QString new_password = ui->lineEdit_confirm_password->text();//确认密码
    QString phone_number=ui->lineEdit_phone_number->text();//手机号
    QString code=ui->lineEdit_code->text();//验证码
    if(password==new_password)
    {
        emit signup_request(username,password,phone_number,code);
    }
    else
    {
        QMessageBox::information(this,"注册","密码不匹配");
    }

}

void Signup::on_checkBox_confirm_password_lineEdit_stateChanged(int arg1)//确定密码可见框(槽函数)
{
    if(arg1)
        ui->lineEdit_confirm_password->setEchoMode(QLineEdit::Normal);    
    else   
        ui->lineEdit_confirm_password->setEchoMode(QLineEdit::Password);

}

void Signup::on_checkBox_password_lineEdit_stateChanged(int arg1)//密码可见框(槽函数)
{
    if(arg1)    
        ui->lineEdit_password->setEchoMode(QLineEdit::Normal);   
    else    
        ui->lineEdit_password->setEchoMode(QLineEdit::Password);

}

void Signup::Getcode_Respond(unsigned int status)//获取验证码回复处理
{
    if(status==GETCODE_TRUE)
        QMessageBox::information(this,"获取验证码","验证码发送成功");
    else
        QMessageBox::information(this,"获取验证码","验证码发送失败");
}

void Signup::Signup_Respond(unsigned int status)//注册回复处理
{
    switch(status)
    {
    case SIGNUP_TRUE:
    {
        QMessageBox::information(this,"注册","注册成功");
        break;
    }
    case SIGNUP_FALSE_ACCOUNT:
    {
        QMessageBox::information(this,"注册","注册失败:账号已被注册");
        break;
    }
    case SIGNUP_FALSE_CODE:
    {
        QMessageBox::information(this,"注册","注册失败:验证码不正确");
        break;
    }
    }
    this->close();
}
