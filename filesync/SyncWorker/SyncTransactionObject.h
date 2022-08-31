#ifndef SYNCTRANSACTIONOBJECT_H
#define SYNCTRANSACTIONOBJECT_H

#include "SyncWorker_global.h"
#include <QByteArray>
#include <QDataStream>

enum SyncTask {
    CreateNew,
    Modify,
    Delete
};

struct SYNCWORKER_EXPORT SyncTransactionObject
{
    SyncTask task;
    QString path;
    QByteArray buffer;
};


SYNCWORKER_EXPORT QDataStream& operator<<(QDataStream& output, const SyncTransactionObject& obj);
SYNCWORKER_EXPORT QDataStream& operator>>(QDataStream& input, SyncTransactionObject& obj);


#endif // SYNCTRANSACTIONOBJECT_H
