#include "SyncTransactionObject.h"


QDataStream& operator<<(QDataStream& output, const SyncTransactionObject& obj)
{
    output << obj.task << obj.path << obj.buffer << obj.hash;
    return output;
}

QDataStream& operator>>(QDataStream& input, SyncTransactionObject& obj)
{
    input >> obj.task >> obj.path >> obj.buffer >> obj.hash;
    return input;
}
