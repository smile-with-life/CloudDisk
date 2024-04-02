#include "ReceiveData.h"
#include "ReceiveSocket.h"
#include "protocol.h"
#include<QThreadPool>

#include <QFileInfo>

ReceiveSocket::ReceiveSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
}

ReceiveSocket::~ReceiveSocket()
{

}

void ReceiveSocket::recvMsg()
{
    if(!m_UpLoadFlag)
    {
        unsigned int TCP_PDULength=0;
        this->read((char*)&TCP_PDULength,sizeof(unsigned int));
        unsigned int MessageLength=TCP_PDULength-sizeof(TCP_PDU);

        TCP_PDU* pdu = ApplyTCP_PDU(MessageLength);
        this->read((char*)pdu+sizeof(unsigned int),TCP_PDULength-sizeof(unsigned int));
        if(pdu->MessageType==UPLOAD_FILE_REQUEST_MESSAGE)
        {
            char Filename[64]={'\0'};
            char strFilesize[64];
            strncpy(Filename,pdu->Data,64);
            strncpy(strFilesize,pdu->Data+64,64);
            m_UploadPath=QString("%1/%2").arg((char*)pdu->Message,Filename);
            m_Filesize=QString(strFilesize).toInt();
            m_Recvsize=0;
            QFile file(m_UploadPath);
            file.open(QIODevice::WriteOnly);//创建文件
            file.close();
            m_UpLoadFlag=true;

        }
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QByteArray Buffer = readAll();
        ReceiveData *Data=new ReceiveData(m_UploadPath,Buffer);
        QThreadPool::globalInstance()->start(Data);
        m_Recvsize+=Buffer.size();
        if(m_Filesize==m_Recvsize)
        {
            m_UpLoadFlag=false;
            qDebug()<<"所有任务正在执行";
            emit offline(this);
        }
        else if(m_Filesize<m_Recvsize)
        {
            m_UpLoadFlag=false;
            qDebug()<<"错误";
            emit offline(this);
        }

    }


}
