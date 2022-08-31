#include "SyncServiceFinder.h"
#include <QNetworkDatagram>
#include <QApplication>
#include <QElapsedTimer>

#include "SyncerCommon.h"

static constexpr qint64 FINDING_TIME_OUT = 3000;

SyncServiceFinder::SyncServiceFinder(QObject* parent)
    : QObject{parent},
    mLowerPort(50000),
    mUpperPort(60000)
{
    setAutoDelete(false);
}

SyncServiceFinder::~SyncServiceFinder()
{
    requestInterruption();
    wait();
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
    qDebug() << "Start";
    QElapsedTimer elapsedTimer;
    elapsedTimer.restart();
    QUdpSocket* socket = new QUdpSocket();
    socket->bind(0, QUdpSocket::ShareAddress);
    mEndPoints.clear();
    for (uint16_t port = mLowerPort; port <= mUpperPort; ++port) {
        QNetworkDatagram datagram(FINDER_HEADER_DATAGRAM.toLocal8Bit(), QHostAddress::Broadcast, port);
        socket->writeDatagram(datagram);
    }

    while(!elapsedTimer.hasExpired(FINDING_TIME_OUT) && !isInterruptionRequested()) {
        if(socket->waitForReadyRead(5)) {
            readPendingDatagram(socket);
        }
    }
    delete socket;
    qDebug() << "End" << elapsedTimer.elapsed();
    Q_EMIT searchFinished();
}

bool SyncServiceFinder::isFind()
{
    return mEndPoints.size() > 0;
}

QVector<SyncServiceFinder::EndPoint> SyncServiceFinder::getFoundEndPoints() const
{
    QVector<EndPoint> endpoints;
    for (auto iter = mEndPoints.begin(); iter != mEndPoints.end(); ++iter) {
        endpoints.append(*iter);
    }
    return endpoints;
}

void SyncServiceFinder::readPendingDatagram(QUdpSocket *socket)
{
    while(socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        uint16_t port;
        QHostAddress hostAddr;
        qint64 len = socket->readDatagram(datagram.data(), datagram.size(), &hostAddr, &port);
        if (len == datagram.size()) {
            const QString received(datagram);
            if (received == PROVIDER_HEADER_DATAGRAM) {
                mEndPoints.insert(hostAddr.toIPv4Address(), {hostAddr, port});
                qDebug() << "Server addr:" << hostAddr.toString() << port << "datagram:" << datagram;
            }
        }
    }
}

void SyncServiceFinder::worker()
{
    executeSearch();
}
