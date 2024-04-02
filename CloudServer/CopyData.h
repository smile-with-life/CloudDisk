#ifndef COPYDATA_H
#define COPYDATA_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRunnable>
#include <QString>

class CopyData : public QRunnable
{
private:
    QString m_SourcePath;
    QString m_DestPath;
public:
    explicit CopyData(QString SourcePath,QString DestPath)
        : m_SourcePath(SourcePath)
        , m_DestPath(DestPath)
    {

    }
    void run() override
    {
        setAutoDelete(true);
        QFile::copy(m_SourcePath,m_DestPath);

    }
};

#endif // COPYDATA_H
