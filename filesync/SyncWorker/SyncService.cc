#include "SyncService.h"
#include <QDebug>
#include "SyncTransactionTaskWorker.h"

SyncService::SyncService(QObject *parent)
    : QThread{parent},
      mStart(false),
      mIsError(false)
{
    mFilesystemWatcher = new SyncFilesystemWatcher(this);
    connect(mFilesystemWatcher->dispatcher(), &SyncFilesystemWatcherDispatcher::fileChanged, this, &SyncService::onFileChanged);
    mTaskCreator = new SyncTransactionTaskCreator(this);
}

void SyncService::setRootPath(const QString &path)
{
    mRootPath = path;
    mFilesystemWatcher->setWatchPath(path);
    mTaskCreator->setRootPath(path);
    SyncTransactionTaskWorker::GetInstance()->setRootPath(path);
}

QString SyncService::rootPath() const
{
    return mRootPath;
}

void SyncService::setServerAddr(const QHostAddress &server)
{
    mServerAddr = server;
}

QHostAddress SyncService::serverAddr() const
{
    return mServerAddr;
}

void SyncService::setPort(int port)
{
    mPort = port;
}

int SyncService::port() const
{
    return mPort;
}

bool SyncService::isError()
{
    return mIsError;
}

bool SyncService::start()
{
    mStart = true;
    mIsError = false;
    return true;
}

void SyncService::stop()
{
    mStart = false;
}

void SyncService::sendChangeToServer(const FileChangeObject &obj)
{
    QTcpSocket* socket = new QTcpSocket();
    socket->connectToHost(mServerAddr, mPort);
    mPool.start(new SyncFileToServerWorker(socket, mTaskCreator, obj));
}

void SyncService::onFileChanged()
{
    FileChangeObject obj;
    if (mFilesystemWatcher->dispatcher()->pop(obj)) {
        if (mStart) {
            sendChangeToServer(obj);
        }
    }
}

SyncFileToServerWorker::SyncFileToServerWorker(QTcpSocket* socket, SyncTransactionTaskCreator *creator, const FileChangeObject &obj)
{
    mSocket = socket;
    mChangeObj = obj;
    mCreator = creator;
}

void SyncFileToServerWorker::worker()
{
    // TODO create the task and sync;
    mSocket->waitForConnected();
    if (!mSocket->isOpen()) {
        return;
    }
    SyncTransactionObject object;
    bool createState;
    switch (mChangeObj.task) {
    case CreateNew:
        createState = mCreator->createNewTask(mChangeObj.path, object);
        break;
    case Modify:
        createState = mCreator->createModifyTask(mChangeObj.path, object);
        break;
    case Delete:
        createState = mCreator->createDeleteTask(mChangeObj.path, object);
        break;
    }

    if (!createState) {
        mSocket->disconnectFromHost();
        delete mSocket;
    } else {
        QByteArray buff;
        QDataStream out(&buff, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_15);
        out.startTransaction();
        out << object;
        out.commitTransaction();
        out.device()->seek(0);
        mSocket->write(buff);
    }
    mSocket->disconnectFromHost();
    if (mSocket->state() != QAbstractSocket::UnconnectedState) {
        mSocket->waitForDisconnected();
    }
    delete mSocket;
}
