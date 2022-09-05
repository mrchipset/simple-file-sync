#include "SyncClientWorker.h"

SyncClientWorker::SyncClientWorker(const SyncTransactionObject &taskObject, QObject *parent) :  QObject(parent),
    SyncWorker(),
    mTaskObject(taskObject)
{

}

void SyncClientWorker::worker()
{

}
