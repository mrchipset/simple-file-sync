#ifndef SYNCSERVERWORKER_H
#define SYNCSERVERWORKER_H

#include <QTcpSocket>
#include <QObject>

#include "SyncWorker.h"
#include "SyncTransactionObject.h"

class SyncServerWorker : public QObject, public SyncWorker
{
    Q_OBJECT
public:
    explicit SyncServerWorker(qintptr socketDescriptor, QObject *parent = nullptr);

Q_SIGNALS:
    void error(QTcpSocket::SocketError socketError);
private:
    qintptr mSocketDescriptor;

    bool getContent(QByteArray& content);
    bool parsingContentTask(const QByteArray& content, SyncTransactionObject& obj);
protected:
    virtual void worker() override;
};

#endif // SYNCSERVERWORKER_H
