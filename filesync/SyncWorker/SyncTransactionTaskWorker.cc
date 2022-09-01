#include "SyncTransactionTaskWorker.h"
#include <QCoreApplication>
#include <QDir>
#include <QMutexLocker>

static constexpr int MAX_QUEUE_SIZE = 16;
static constexpr int THREAD_SLEEP_TIME = 100;

SyncTransactionTaskWorker* SyncTransactionTaskWorker::Instance = nullptr;

SyncTransactionTaskWorker::SyncTransactionTaskWorker(QObject* parent) : QThread(parent)
{

}

SyncTransactionTaskWorker *SyncTransactionTaskWorker::GetInstance()
{
    if (Instance == nullptr) {
        Instance = new SyncTransactionTaskWorker(QCoreApplication::instance());
    }
    return Instance;
}

void SyncTransactionTaskWorker::setRootPath(const QString &path)
{
    mRootPath = path;
}

QString SyncTransactionTaskWorker::rootPath() const
{
    return mRootPath;
}

bool SyncTransactionTaskWorker::doTaskWork(const SyncTransactionObject &taskObject)
{
    QMutexLocker lk(&mMutex);
    if (mTaskObjs.size() > MAX_QUEUE_SIZE) {
        return false;
    } else {
        mTaskObjs.enqueue(taskObject);
        return true;
    }
}

bool SyncTransactionTaskWorker::dispatchTask(const SyncTransactionObject &taskObject)
{
    switch(taskObject.task) {
    case CreateNew:
        return createNewTask(taskObject);
    case Modify:
        return modifyTask(taskObject);
    case Delete:
        return deleteTask(taskObject);
    }
    return true;
}

bool SyncTransactionTaskWorker::createNewTask(const SyncTransactionObject &taskObject)
{
    // check path existed
    QString path = mRootPath + QDir::separator() + taskObject.path;
    if (checkExisted(path)) {
        QFile file(path);
        if(!file.remove()) {
            return false;
        }
    }
    // create new image file
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }
    qint64 len = file.write(taskObject.buffer);
    file.close();
    return len == taskObject.buffer.size();
}

bool SyncTransactionTaskWorker::modifyTask(const SyncTransactionObject &taskObject)
{
    // check path existed
    QString path = mRootPath + QDir::separator() + taskObject.path;
    if (checkExisted(path)) {
        QFile file(path);
        if(!file.remove()) {
            return false;
        }
    }

    // create new image file
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }
    qint64 len = file.write(taskObject.buffer);
    file.close();
    return len == taskObject.buffer.size();
}

bool SyncTransactionTaskWorker::deleteTask(const SyncTransactionObject &taskObject)
{
    // check path existed
    QString path = mRootPath + QDir::separator() + taskObject.path;
    if (checkExisted(path)) {
        QFile file(path);
        if(!file.remove()) {
            return false;
        }
    }
    return true;
}

bool SyncTransactionTaskWorker::checkExisted(const QString &path)
{
    QFile file(path);
    return file.exists();
}

void SyncTransactionTaskWorker::run()
{
    bool isEmpty = true;
    SyncTransactionObject obj;
    while(!isInterruptionRequested()) {
        isEmpty = false;
        {
            QMutexLocker lk(&mMutex);
            if (mTaskObjs.isEmpty()) {
                isEmpty = true;
            } else {
                isEmpty = false;
                obj = mTaskObjs.dequeue();
            }
        }

        if (isEmpty) {
            QThread::sleep(THREAD_SLEEP_TIME);
        } else {
            dispatchTask(obj);
        }
    }
}
