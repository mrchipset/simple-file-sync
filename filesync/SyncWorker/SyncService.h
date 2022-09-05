#ifndef SYNCSERVICE_H
#define SYNCSERVICE_H

#include "SyncWorker_global.h"


#include <QThread>
#include <QThreadPool>
#include <QTcpSocket>
#include <QHostAddress>

#include "SyncFilesystemWatcher.h"
#include "SyncWorker.h"
#include "SyncTransactionTaskCreator.h"

class SYNCWORKER_EXPORT SyncService : public QThread
{
    Q_OBJECT
public:
    explicit SyncService(QObject *parent = nullptr);

    void setRootPath(const QString& path);
    QString rootPath() const;

    void setServerAddr(const QHostAddress& server);
    QHostAddress serverAddr() const;

    void setPort(int port);
    int port() const;

    bool isError();
    bool start();
    void stop();
private:
    SyncFilesystemWatcher* mFilesystemWatcher;
    SyncTransactionTaskCreator* mTaskCreator;
    QString mRootPath;
    QHostAddress mServerAddr;
    QThreadPool mPool;
    int mPort;
    bool mStart;
    bool mIsError;

    void sendChangeToServer(const FileChangeObject& obj);
private Q_SLOTS:
    void onFileChanged();
};

class SyncFileToServerWorker : public SyncWorker
{

public:
    SyncFileToServerWorker(QTcpSocket* socket, SyncTransactionTaskCreator* creator, const FileChangeObject& obj);
    // SyncWorker interface
protected:
    void worker();
private:
    FileChangeObject mChangeObj;
    QTcpSocket* mSocket;
    SyncTransactionTaskCreator* mCreator;
};

#endif // SYNCSERVICE_H
