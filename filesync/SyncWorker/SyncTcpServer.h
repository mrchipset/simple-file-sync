#ifndef SYNCTCPSERVER_H
#define SYNCTCPSERVER_H

#include "SyncWorker_global.h"

#include <QTcpServer>
#include <QObject>
#include <QThreadPool>

class SYNCWORKER_EXPORT SyncTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SyncTcpServer(QObject *parent = nullptr);

private:
    QThreadPool mWorkerPool;
protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;

};

#endif // SYNCERTCPSERVER_H
