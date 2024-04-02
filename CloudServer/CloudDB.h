#ifndef CLOUDDB_H
#define CLOUDDB_H

#include <QObject>
#include<QSqlDatabase>
#include<QSqlQuery>
#include <QTimer>
#include "protocol.h"

class CloudDB : public QObject
{
    Q_OBJECT
private:
    QTimer m_timer;

    QSqlDatabase m_DB;//连接数据库
public:
    explicit CloudDB(QObject *parent = nullptr);

    ~CloudDB();

    static CloudDB& getInstance();//获取单例模式

    void initDB();//初始化数据库

    STATUS logon(QString account,QString password);//数据库处理登录操作

    QString getInformation(QString account);

    void addcode(QString phone_number,QString code);//数据库处理添加验证码操作

    STATUS signup(QString username,QString password,QString phone_number,QString code);//数据库处理注册操作

    void timed_deletion();//定时删除

    void offonline(QString account);//用户离线处理

    void shutdown();//数据库关闭处理


signals:

public slots:

private:

};

#endif // CLOUDDB_H
