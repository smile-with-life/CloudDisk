#include "ReceiveWork.h"
#include "protocol.h"

#include <QFile>
#include <QThread>

ReceiveWork::ReceiveWork(QObject *parent)
    :QObject{parent}
{
    Init();
}

ReceiveWork::~ReceiveWork()
{
    if(p_ReceiveSocket!=nullptr)
        p_ReceiveSocket->deleteLater();
}

void ReceiveWork::Init()
{
    m_IP="127.0.0.1";
    m_ReceivePort=9999;

}

void ReceiveWork::LoadConfig()
{

}

void ReceiveWork::Running(const QString SavePath,const QString DownloadPath)
{
    ConnectServer();//连接Receive服务器
    Send_Download_File_Request(SavePath,DownloadPath);//发送上传文件请求
}

void ReceiveWork::ConnectServer()
{
    qDebug()<<"连接Receive服务器";
    //创建Socket
    p_ReceiveSocket=new QTcpSocket;
    //使用IP地址和端口号连接
    p_ReceiveSocket->connectToHost(QHostAddress(m_IP),m_ReceivePort);//连接Receive服务器
    //处理SendSocket发送的信号
    connect(p_ReceiveSocket,&QTcpSocket::connected,this,&ReceiveWork::Server_Online);
    connect(p_ReceiveSocket,&QTcpSocket::disconnected,this,&ReceiveWork::Server_Offline);
    connect(p_ReceiveSocket,&QTcpSocket::readyRead,this,&ReceiveWork::Receive_File);
    while(!p_ReceiveSocket->waitForConnected());

}

void ReceiveWork::Send_Download_File_Request(const QString SavePath,const QString DownloadPath)
{
    qDebug()<<"发送下载文件请求";
    m_SavePath=SavePath;
    qDebug()<<m_SavePath;
    TCP_PDU*pdu=ApplyTCP_PDU(DownloadPath.toUtf8().length()+1);
    pdu->MessageType=DOWNLOAD_FILE_REQUEST_MESSAGE;
    memcpy(pdu->Message,DownloadPath.toStdString().c_str(),DownloadPath.toUtf8().length()+1);
    p_ReceiveSocket->write((char*)pdu,pdu->TCP_PDULength);
    free(pdu);
    pdu=NULL;

}

void ReceiveWork::Receive_File()
{
    //qDebug()<<"1";
    if(!m_DownloadFlag)
    {
        unsigned int TCP_PDULength=0;
        p_ReceiveSocket->read((char*)&TCP_PDULength,sizeof(unsigned int));
        unsigned int MessageLength=TCP_PDULength-sizeof(TCP_PDU);

        TCP_PDU* pdu = ApplyTCP_PDU(MessageLength);
        p_ReceiveSocket->read((char*)pdu+sizeof(unsigned int),TCP_PDULength-sizeof(unsigned int));
        if(pdu->MessageType==DOWNLOAD_FILE_RESPOND_MESSAGE)
        {

            char strFilesize[64];
            strncpy(strFilesize,pdu->Data,64);
            m_Filesize=QString(strFilesize).toInt();
            qDebug()<<m_Filesize;
            m_Recvsize=0;
            m_file.setFileName(m_SavePath);
            m_file.open(QIODevice::WriteOnly);//创建文件
            m_DownloadFlag=true;
        }
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QByteArray Buffer = p_ReceiveSocket->readAll();
        m_file.write(Buffer);
        m_Recvsize+=Buffer.size();
        //qDebug()<<m_Recvsize;
        emit send_percent((m_Recvsize*100)/m_Filesize);
        if(m_Filesize==m_Recvsize)
        {
            m_DownloadFlag=false;
            m_file.close();
            p_ReceiveSocket->deleteLater();
        }
        else if(m_Filesize<m_Recvsize)
        {
            m_DownloadFlag=false;
            qDebug()<<"错误";
            m_file.close();
            p_ReceiveSocket->deleteLater();
        }
    }

}

void ReceiveWork::Server_Online()
{
    qDebug()<<"服务器连接成功";
    emit server_connected(this);

}

void ReceiveWork::Server_Offline()
{
    qDebug()<<"服务器断开连接";
    emit server_disconnect(this);

}
