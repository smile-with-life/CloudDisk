#ifndef SENDWORK_H
#define SENDWORK_H

#include <QObject>
#include <QTcpSocket>
#include<QTimer>

class SendWork : public QObject
{
    Q_OBJECT
private:
    QString m_IP;//IP地址
    quint16 m_SendPort;//Send端口号
    QTcpSocket *p_SendSocket=nullptr;
    bool m_SendConnectFlag=false;
    QString m_UploadFilePath;
public:
    explicit SendWork(QObject *parent = nullptr);

    ~SendWork();

    void Init();//初始化

    void LoadConfig();//加载配置文件
signals:
    void server_connected(QObject *myWork);//服务器连接成功信号

    void server_disconnect(QObject *myWork);//服务器断开连接信号

    void send_percent(int percent);//发送进度信号

    void upload_file_respond(unsigned int Status);//上传文件回复信号

public slots:
    void Running(const QString CurrentPath,const QString UploadFielPath);

private:
    void ConnectServer();

    void Send_Upload_File_Request(const QString CurrentPath,const QString UploadFilePath);//上传文件请求

    void Send_File();

    void Server_Online();

    void Server_Offline();
};

#endif // SENDWORK_H
