#include "SyncTransactionObject.h"


QDataStream& operator<<(QDataStream& output, const SyncTransactionObject& obj)
{
    output << obj.task << obj.path << obj.buffer;
    return output;
}

QDataStream& operator>>(QDataStream& input, SyncTransactionObject& obj)
{
    input >> obj.task >> obj.path >> obj.buffer;
    return input;
}
