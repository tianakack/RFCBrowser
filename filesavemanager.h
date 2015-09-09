#ifndef FILESAVEMANAGER_H
#define FILESAVEMANAGER_H

#include <QNetworkAccessManager>

class FileSaveManager : public QObject
{
    Q_OBJECT

public:
    FileSaveManager(QObject *parent = 0);
    ~FileSaveManager();

    QString mFileName;
    QString mFilePathName;

    void setFileName(const QString &fileName);
    void setFilePathName(const QString &filePathName);
    void exec();

signals:
    savedDone(QString mFileName);

private slots:
    void onNetworkFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager mNetwork;

};

#endif // FILESAVEMANAGER_H
