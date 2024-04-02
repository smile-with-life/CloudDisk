#include "clouddb.h"
#include <QDate>
#include <QMessageBox>

CloudDB::CloudDB(QObject *parent)
    : QObject{parent}
{
    m_DB=QSqlDatabase::addDatabase("QSQLITE");
    //开启定时器
    m_timer.start(1000);
    //信号类型:处理计时器信号
    connect(&m_timer, &QTimer::timeout, this,[=](){timed_deletion();m_timer.start(1000);} );
}

CloudDB::~CloudDB()
{

    shutdown();
    m_DB.close();
}

CloudDB &CloudDB::getInstance()//获取单例模式
{
    static CloudDB instance;
    return instance;
}

void CloudDB::initDB()//初始化数据库
{
    //连接数据库
    m_DB.setHostName("localhost");
    m_DB.setDatabaseName("D:\\Proceduct\\Qt Project\\build-CloudServer-MinGW_11_2_0_64_bit-Debug\\cloud.db");
    //打开数据库
    if(m_DB.open())
        qDebug()<<"数据库连接成功";
    else
        QMessageBox::information(NULL,"连接数据库","连接失败");

}

STATUS CloudDB::logon(QString account,QString password)//数据库处理登录操作
{
    qDebug()<<"数据库处理登录操作";
    QSqlQuery query(m_DB);
    QString statement=QString("select * from userinfo where phone_number='%1' and password='%2'").arg(account,password);
    //qDebug()<<statement;
    query.exec(statement);
    if(query.next())
    {
        int online=query.value(4).toInt();
        if(online==0)
        {
            statement=QString("update userinfo set online=1 where phone_number='%1'").arg(account);
            query.exec(statement);
            qDebug()<<account<<"允许登录";
            return LOGON_TRUE;//允许登录
        }
        else
        {
            qDebug()<<account<<"用户已登录";
            return LOGON_FALSE_ONLINE;//用户已登录
        }

    }
    else
    {
        qDebug()<<"账号或密码不正确";
        return LOGON_FALSE_ACCOUNT;//账号或密码不正确
    }

}

QString CloudDB::getInformation(QString account)//数据库获取信息操作
{
    QSqlQuery query(m_DB);
    QString statement=QString("select * from userinfo where phone_number='%1';").arg(account);
    query.exec(statement);
    if(query.next())
    {
        return query.value(0).toString();
    }
    else
    {
        return "error";
    }

}

void CloudDB::addcode(QString phone_number,QString code)//数据库处理获取验证码操作
{
    qDebug()<<"数据库处理获取验证码操作";
    QSqlQuery query(m_DB);
    QString statement=QString("insert into codeinfo values('%1','%2',60);").arg(phone_number,code);
    //qDebug()<<statement;
    query.exec(statement);
}

STATUS CloudDB::signup(QString username, QString password, QString phone_number, QString code)//数据库处理注册操作
{
    // qDebug()<<username;
    // qDebug()<<password;
    // qDebug()<<phone_number;
    // qDebug()<<code;
    qDebug()<<"数据库处理注册操作";
    QSqlQuery query(m_DB);
    QString statement=QString("select * from codeinfo where phone_number='%1' and code='%2'").arg(phone_number,code);
    //qDebug()<<statement;
    query.exec(statement);
    if(query.next())
    {
        //从验证码表中删除失效记录
        statement=QString("delete * from codeinfo where phone_number='%1'").arg(phone_number);
        query.exec(statement);
        //查询账号是否被注册
        statement=QString("select * from userinfo where phone_number='%1'").arg(phone_number);
        query.exec(statement);
        if(query.next())
        {
            qDebug()<<"账号已被注册";
            return SIGNUP_FALSE_ACCOUNT;//账号已被注册
        }
        else
        {
            //获取当前日期
            QDate date(QDate::currentDate());
            QString start_time=date.toString("yyyy-MM-dd");
            //向用户表中写入新的用户信息
            statement=QString("insert into userinfo(username,password,phone_number,start_time) values('%1','%2','%3','%4'); ").arg(username,password,phone_number,start_time);
            query.exec(statement);
            qDebug()<<username<<"注册成功";
            return SIGNUP_TRUE;//注册成功
        }
    }
    else
    {
        qDebug()<<"验证码不正确";
        return SIGNUP_FALSE_CODE;//验证码不正确
    }
}

void CloudDB::timed_deletion()//定时更新数据库
{
    //执行更新
    QSqlQuery query(m_DB);
    QString statement=QString("update codeinfo set duration = duration - 1 where 1;");
    query.exec(statement);
    //清理无效数据
    statement=QString("delete from codeinfo where duration <= 0");
    query.exec(statement);

}

void CloudDB::offonline(QString account)//数据库处理用户离线
{
    qDebug()<<"用户离线处理";
    QString statement = QString("update userinfo set online = 0 where username='%1' or phone_number='%1'").arg(account);
    //qDebug()<<statement;
    QSqlQuery query(m_DB);
    query.exec(statement);
}

void CloudDB::shutdown()//数据库关闭处理
{
    qDebug()<<"数据库关闭处理";
    QSqlQuery query(m_DB);
    //把所有用户都设置为离线
    query.exec("update userinfo set online = 0 where 1");
    //把验证码表所有数据项全部删除
    query.exec("delete from codeinfo where 1");
}

