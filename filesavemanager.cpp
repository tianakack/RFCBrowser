#include <QtDebug>
#include <QFile>
#include <QNetworkReply>
#include "filesavemanager.h"

FileSaveManager::FileSaveManager(QObject *parent) :
    QObject(parent)
{
    qDebug() << "FileSaveManager::FileSaveManager()";

    connect(&mNetwork, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkFinished(QNetworkReply*)));
}

FileSaveManager::~FileSaveManager()
{
    qDebug() << "FileSaveManager::~FileSaveManager()";
}

void FileSaveManager::setFileName(const QString &fileName)
{
    mFileName = fileName;
}

void FileSaveManager::setFilePathName(const QString &filePathName)
{
    mFilePathName = filePathName;
}

void FileSaveManager::exec()
{
    mNetwork.get(QNetworkRequest(QUrl(QString("http://tools.ietf.org/rfc/%1").arg(mFileName))));
}

void FileSaveManager::onNetworkFinished(QNetworkReply *reply)
{
    QFile file(mFilePathName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(reply->readAll());

        file.close();
    }

    emit savedDone(mFileName);

    deleteLater();
}

