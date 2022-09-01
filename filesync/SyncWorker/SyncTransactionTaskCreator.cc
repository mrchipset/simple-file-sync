#include "SyncTransactionTaskCreator.h"
#include <QDir>
#include <QCryptographicHash>
SyncTransactionTaskCreator::SyncTransactionTaskCreator(QObject *parent)
    : QObject{parent}
{

}

void SyncTransactionTaskCreator::setRootPath(const QString &path)
{
    mRootPath = path;
}

QString SyncTransactionTaskCreator::rootPath() const
{
    return mRootPath;
}

bool SyncTransactionTaskCreator::createNewTask(const QString &path, SyncTransactionObject &taskObj)
{
    taskObj.task = CreateNew;
    taskObj.path = path;
    return readFile(path, taskObj);
}

bool SyncTransactionTaskCreator::createModifyTask(const QString &path, SyncTransactionObject &taskObj)
{
    taskObj.task = Modify;
    taskObj.path = path;
    return readFile(path, taskObj);
}

bool SyncTransactionTaskCreator::createDeleteTask(const QString &path, SyncTransactionObject &taskObj)
{
    taskObj.task = Delete;
    taskObj.path = path;
    return true;
}

bool SyncTransactionTaskCreator::readFile(const QString &path, SyncTransactionObject& taskObj)
{
    QString filePtah = mRootPath + QDir::separator() + path;
    QFile file(filePtah);
    taskObj.buffer.clear();
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    taskObj.buffer = file.readAll();
    file.close();
    taskObj.hash = QCryptographicHash::hash(taskObj.buffer, QCryptographicHash::Sha1);
    return true;
}
