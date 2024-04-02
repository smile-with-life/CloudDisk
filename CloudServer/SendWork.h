#ifndef SENDWORK_H
#define SENDWORK_H

#include <QObject>
#include <QTcpSocket>

class SendWork : public QObject
{
    Q_OBJECT
private:
    QTcpSocket *p_TcpSocket=nullptr;
    QString m_DownloadPath;
public:
    explicit SendWork(QObject *parent = nullptr);

    ~SendWork();
signals:
    void finish();

public slots:
    void Start(qintptr socketDescriptor);
private slots:
    void recvMsg();
};

#endif // SENDWORK_H
