#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <Work.h>

namespace Ui {
class Signup;
}
class Network;

class Signup : public QWidget
{
    Q_OBJECT
private:
    QTimer*p_timer=nullptr;
    int m_time_interval=0;//重发间隔
public:
    explicit Signup(QWidget *parent = nullptr);//构造函数

    ~Signup();//析构函数

    void Signupshow();//显示

    void Signupclose();//关闭
private:
    void Init();//初始化

    void hide_widget();

    void lock_button(int time_interval);

signals:
    void getcode_request(QString phone_number);//获取验证码信号

    void signup_request(QString username,QString password,QString phone_number,QString code);//注册信号
public slots:
    void time_show();

    void Getcode_Respond(unsigned int status);//获取验证码回复处理

    void Signup_Respond(unsigned int status);//注册回复处理
private slots:
    void on_pushButton_signup_clicked();//注册按钮(槽函数)

    void on_pushButton_getcode_clicked();//获取验证码(槽函数)

    void on_checkBox_confirm_password_lineEdit_stateChanged(int arg1);//确定密码可见框(槽函数)

    void on_checkBox_password_lineEdit_stateChanged(int arg1);//密码可见框(槽函数)

private:
    Ui::Signup *ui;
};

#endif // SIGNUP_H
