#ifndef RECEIVEWORK_H
#define RECEIVEWORK_H

#include <QFile>
#include <QObject>
#include <QTcpSocket>


class ReceiveWork:public QObject
{
    Q_OBJECT
private:
    QString m_IP;//IP地址
    quint16 m_ReceivePort;//SendServer端口号
    QTcpSocket *p_ReceiveSocket=nullptr;
    bool m_ReceiveConnectFlag=false;
    bool m_DownloadFlag=false;
    QFile m_file;
    QString m_SavePath;
    qint64 m_Filesize;
    qint64 m_Recvsize;
public:
    explicit ReceiveWork(QObject *parent = nullptr);

    ~ReceiveWork();

    void Init();//初始化

    void LoadConfig();//加载配置文件
signals:
    void server_connected(QObject *myWork);//服务器连接成功信号

    void server_disconnect(QObject *myWork);//服务器断开连接信号

    void send_percent(int percent);//发送进度信号

    void download_file_respond(unsigned int Status);//下载文件回复信号
public slots:
    void Running(const QString SavePath,const QString DownloadPath);
private:
    void ConnectServer();

    void Send_Download_File_Request(const QString SavePath,const QString DownloadPath);//上传文件请求

    void Receive_File();

    void Server_Online();

    void Server_Offline();
};

#endif // RECEIVEWORK_H
