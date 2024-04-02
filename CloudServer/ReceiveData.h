#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QFile>
#include <QRunnable>
#include <QString>
#include <QDebug>


class ReceiveData : public QRunnable
{
private:
    QString m_FilePath;
    QByteArray m_Buffer;
public:
    explicit ReceiveData(QString FilePath,QByteArray Buffer)
        : m_FilePath(FilePath)
        , m_Buffer(Buffer)
    {

    }
    void run() override
    {
        setAutoDelete(true);
        QFile file(m_FilePath);
        file.open(QIODevice::Append);
        file.write(m_Buffer);
        file.close();
    }
};

#endif // RECEIVEDATA_H
