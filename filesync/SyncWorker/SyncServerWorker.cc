#include "SyncServerWorker.h"
#include <QDataStream>
#include <QThread>
#include "SyncTransactionTaskWorker.h"

static constexpr int WAIT_READ_RETRY_LIMIT = 3;
static constexpr int TRANSACTION_RETRY_LIMIT = 5;
static constexpr int WAIT_READ_READY_TIMEOUT = 100;
SyncServerWorker::SyncServerWorker(qintptr socketDescriptor, QObject *parent)
    : QObject{parent},
      mSocketDescriptor(socketDescriptor)
{

}

bool SyncServerWorker::getContent(QByteArray &content)
{
    QTcpSocket socket;
    bool ret = true;
    if(!socket.setSocketDescriptor(mSocketDescriptor)) {
        Q_EMIT error(socket.error());
    }

    int transactionRetry = 0;
    int waitReadRetry = 0;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_5_15);
    do {
        if (!socket.waitForReadyRead(WAIT_READ_READY_TIMEOUT)) {
            waitReadRetry++;
            if (waitReadRetry > WAIT_READ_RETRY_LIMIT) {
                Q_EMIT error(socket.error());
                ret = false;
                break;
            }

        }
        if (transactionRetry > TRANSACTION_RETRY_LIMIT) {
            ret = false;
            break;
        }
        transactionRetry++;
        in.startTransaction();
        in >> content;
    } while(!in.commitTransaction());
    socket.disconnectFromHost();
    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.waitForDisconnected();
    }
    return ret;
}

bool SyncServerWorker::parsingContentTask(const QByteArray &content, SyncTransactionObject &obj)
{
    QDataStream in(content);
    in.startTransaction();
    in >> obj;
    return in.commitTransaction();
}

void SyncServerWorker::worker()
{
    QByteArray content;
    if(!getContent(content)) {
        qDebug() << "Get content error.";
        return;
    }

    SyncTransactionObject object;
    if(!parsingContentTask(content, object)) {
        qDebug() << "parsing content task error.";
        return;
    }

    // push the task object to a queue worker
    SyncTransactionTaskWorker* taskWorker = SyncTransactionTaskWorker::GetInstance();
    taskWorker->doTaskWork(object);
}
