#ifndef LOGON_H
#define LOGON_H
#include <Information.h>
#include <QTimer>
#include <QWidget>

namespace Ui {
class Logon;
}

class Logon : public QWidget
{
    Q_OBJECT
private:
    QMovie *p_movie=nullptr;
    QTimer *p_logontimer=nullptr;//定时发送登录信号定时器
    Information *sp_info=nullptr;
    bool m_LogonState=false;
public:
    explicit Logon(QWidget *parent = nullptr);
    ~Logon();
    void Logonshow(QString account,QString password);//进行登录

    void Logonclose();//关闭处理
private:
    void Init();//初始化
signals:
    void logon_request(QString account,QString password);//登录请求信号

    void logoning();//登录成功信号
private slots:
    void on_pushButton_cancel_clicked();
public slots:
    void Logon_Respond(const unsigned int status,const QString username,const QString phone_number);

private:
    Ui::Logon *ui;
};

#endif // LOGON_H
