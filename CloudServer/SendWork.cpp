#include "SendWork.h"
#include "protocol.h"

#include <QFileInfo>
#include <QTcpSocket>
#include <QThread>

SendWork::SendWork(QObject *parent)
    : QObject{parent}
{

}

SendWork::~SendWork()
{
    if(p_TcpSocket!=nullptr)
        p_TcpSocket->deleteLater();
    emit finish();

}

void SendWork::Start(qintptr socketDescriptor)
{
    p_TcpSocket=new QTcpSocket;//申请一个本机套接字
    qDebug()<<"new tcp connect Task "<<p_TcpSocket;
    p_TcpSocket->setSocketDescriptor(socketDescriptor);//设置本机套接字描述符
    connect(p_TcpSocket,&QTcpSocket::readyRead,this,&SendWork::recvMsg);
    connect(p_TcpSocket,&QTcpSocket::disconnected,this,&SendWork::deleteLater);
}

void SendWork::recvMsg()
{
    unsigned int TCP_PDULength=0;
    p_TcpSocket->read((char*)&TCP_PDULength,sizeof(unsigned int));
    unsigned int MessageLength=TCP_PDULength-sizeof(TCP_PDU);

    TCP_PDU* pdu = ApplyTCP_PDU(MessageLength);
    p_TcpSocket->read((char*)pdu+sizeof(unsigned int),TCP_PDULength-sizeof(unsigned int));
    if(pdu->MessageType==DOWNLOAD_FILE_REQUEST_MESSAGE)
    {
        m_DownloadPath=QString("%1").arg((char*)pdu->Message);
        QFileInfo fileinfo(m_DownloadPath);
        TCP_PDU*respdu=ApplyTCP_PDU(0);
        respdu->MessageType=DOWNLOAD_FILE_RESPOND_MESSAGE;
        strncpy(respdu->Data,QString::number(fileinfo.size()).toStdString().c_str(),64);
        p_TcpSocket->write((char*)respdu,respdu->TCP_PDULength);
        free(respdu);
        respdu=NULL;
    }
    QThread::msleep(1000);
    qDebug()<<"发送文件";
    QFile file(m_DownloadPath);
    qDebug()<<m_DownloadPath;
    if(file.open(QIODevice::ReadOnly))
    {
        char*Buffer=new char[65536];
        qint64 ret=0;//实际读到的数据大小
        while(true)
        {
            ret=file.read(Buffer,65536);
            //qDebug()<<ret;
            if(ret>0 && ret<=65536)
            {
                p_TcpSocket->write(Buffer,ret);
            }
            else if(ret==0)
            {

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
    qDebug()<<"上传文件成功";
    //this->deleteLater();//上传文件成功
}
