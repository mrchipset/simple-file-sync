#include "SyncTcpServer.h"
#include "SyncServerWorker.h"

SyncTcpServer::SyncTcpServer(QObject *parent)
    : QTcpServer{parent}
{
    int count = QThread::idealThreadCount();
    if (count > 4) {
        count = std::min(4, count);
    } else {
        if (count > 2) {
            count--;
        }
    }

    mWorkerPool.setMaxThreadCount(count);
}

void SyncTcpServer::incomingConnection(qintptr socketDescriptor)
{
    mWorkerPool.start(new SyncServerWorker(socketDescriptor));
}
