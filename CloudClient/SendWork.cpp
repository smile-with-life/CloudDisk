#include "SendWork.h"
#include "protocol.h"
#include <QDebug>
#include <QFileDialog>
#include <QTcpSocket>
#include <QThread>

SendWork::SendWork(QObject *parent)
    : QObject{parent}
{
    Init();

}

SendWork::~SendWork()
{
    if(p_SendSocket!=nullptr)
        p_SendSocket->deleteLater();

}

void SendWork::Init()
{
    m_IP="127.0.0.1";
    m_SendPort=8888;

}

void SendWork::LoadConfig()
{

}

void SendWork::Running(const QString CurrentPath, const QString UploadFilePath)//运行
{
    ConnectServer();//连接Receive服务器
    Send_Upload_File_Request(CurrentPath,UploadFilePath);//发送上传文件请求
    QThread::msleep(1000);//阻塞延时1000ms
    Send_File();//发送文件
}

void SendWork::ConnectServer()//连接Receive服务器
{
    qDebug()<<"连接Receive服务器";
    //创建Socket
    p_SendSocket=new QTcpSocket;
    //使用IP地址和端口号连接
    p_SendSocket->connectToHost(QHostAddress(m_IP),m_SendPort);//连接Receive服务器
    //处理SendSocket发送的信号
    connect(p_SendSocket,&QTcpSocket::connected,this,&SendWork::Server_Online);
    connect(p_SendSocket,&QTcpSocket::disconnected,this,&SendWork::Server_Offline);
    while(!p_SendSocket->waitForConnected());

}

void SendWork::Send_Upload_File_Request(const QString CurrentPath, const QString UploadFilePath)//发送上传文件请求
{
    qDebug()<<"发送上传文件请求";
    m_UploadFilePath=UploadFilePath;
    int index=UploadFilePath.lastIndexOf('/');
    QString Filename=UploadFilePath.last(UploadFilePath.size()-index-1);
    QFile file(UploadFilePath);
    qint64 Filesize=file.size();
    TCP_PDU*pdu=ApplyTCP_PDU(CurrentPath.toUtf8().length()+1);
    pdu->MessageType=UPLOAD_FILE_REQUEST_MESSAGE;
    memcpy(pdu->Message,CurrentPath.toStdString().c_str(),CurrentPath.toUtf8().length()+1);
    strncpy(pdu->Data,Filename.toStdString().c_str(),64);
    strncpy(pdu->Data+64,QString::number(Filesize).toStdString().c_str(),64);
    p_SendSocket->write((char*)pdu,pdu->TCP_PDULength);
    free(pdu);
    pdu=NULL;
}

void SendWork::Send_File()//发送文件
{
    qDebug()<<"发送文件";
    QFile file(m_UploadFilePath);
    QFileInfo fileInfo(file);
    qint64 Filesize=fileInfo.size();
    int SendPercent=0;//发送进度
    qint64 CurrentPercent=0;//已发送的数据大小
    if(file.open(QIODevice::ReadOnly))
    {
        char*Buffer=new char[65536];
        qint64 ret=0;//实际读到的数据大小
        while(true)
        {
            ret=file.read(Buffer,65536);
            if(ret>0 && ret<=65536)
            {
                CurrentPercent+=ret;//计算已发送的数据大小
                p_SendSocket->write(Buffer,ret);
                SendPercent = (CurrentPercent*100) / Filesize;//计算总进度
                emit send_percent(SendPercent);//发射send_percent信号，用于在主线程更新进度条
            }
            else if(ret==0)
            {
                //上传文件成功
                break;
            }
            else
            {
                qDebug()<<"上传文件失败：读取文件失败";
                break;
            }
        }
        file.close();
        delete[]Buffer;
        Buffer=NULL;
    }
    else
    {
        qDebug()<<"打开文件失败";
        return;
    }
}

void SendWork::Server_Online()
{
    qDebug()<<"服务器连接成功";

    emit server_connected(this);

}

void SendWork::Server_Offline()
{
    qDebug()<<"服务器断开连接";
    emit server_disconnect(this);

}
