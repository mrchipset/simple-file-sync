#ifndef SYNCCLIENTWORKER_H
#define SYNCCLIENTWORKER_H

#include <QObject>
#include <QTcpSocket>
#include "SyncWorker.h"
#include "SyncTransactionObject.h"

class SyncClientWorker : public QObject, public SyncWorker
{
    Q_OBJECT
public:
    SyncClientWorker(const SyncTransactionObject& taskObject, QObject* parent);


private:
    SyncTransactionObject mTaskObject;
protected:
    void worker();
};

#endif // SYNCCLIENTWORKER_H
