#include "SyncServiceFinder.h"
#include <QUdpSocket>
#include <QNetworkDatagram>

SyncServiceFinder::SyncServiceFinder() :
    mLowerPort(50000),
    mUpperPort(60000)
{

}

void SyncServiceFinder::setPortRange(uint16_t lower, uint16_t upper)
{
    mLowerPort = lower;
    mUpperPort = upper;
}

void SyncServiceFinder::getPortRange(uint16_t &lower, uint16_t &upper)
{
    lower = mLowerPort;
    upper = mUpperPort;
}

void SyncServiceFinder::executeSearch()
{
    QUdpSocket* socket = new QUdpSocket();
    QString header = "FIND-HEADE-MAGIC-1234";
    uint16_t port = 10000;
    QNetworkDatagram datagram(header.toLocal8Bit(), QHostAddress::Broadcast, port);
    socket->writeDatagram(datagram);
    delete socket;
}

bool SyncServiceFinder::isFind()
{
    return true;
}
