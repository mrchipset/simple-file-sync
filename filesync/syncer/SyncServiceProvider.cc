#include "SyncServiceProvider.h"
#include <QNetworkDatagram>

#include "AvailablePortHelper.h"
#include "SyncerCommon.h"
#include "SyncTcpServer.h"

static constexpr uint16_t PORT_LOWER = 50000;
static constexpr uint16_t PORT_UPPER = 60000;

SyncServiceProvider::SyncServiceProvider(QObject *parent)
    : QObject{parent},
      mUdpFinderSocket(nullptr),
      mTcpServer(nullptr)
{
}

bool SyncServiceProvider::createService()
{
    QVector<uint16_t> ports;
    AvailablePortHelper::GetAvailablePorts(PORT_LOWER, PORT_UPPER, ports);
    if (ports.size() > 0) {
        uint16_t port = ports.first();
        mTcpServer = new SyncTcpServer(this);
        if (!mTcpServer->listen(QHostAddress::Any, port)) {
            return false;
        }
        mUdpFinderSocket = new QUdpSocket(this);
        if (!mUdpFinderSocket->bind(port, QUdpSocket::ShareAddress)) {
            return false;
        }

        connect(mUdpFinderSocket, &QUdpSocket::readyRead, this, &SyncServiceProvider::onDatagramPending);
        qDebug() << "Bind to port:" << port;
        return true;
    } else {
        return false;
    }
}

void SyncServiceProvider::onDatagramPending()
{
    while(mUdpFinderSocket->hasPendingDatagrams()) {
        qint64 size = mUdpFinderSocket->pendingDatagramSize();
        QByteArray datagram(size, 0);
        QHostAddress host;
        quint16 port;
        qint64 len = mUdpFinderSocket->readDatagram(datagram.data(), size, &host, &port);
        if (len != size) {
            continue;
        } else {
            QString received(datagram);
            if (received == FINDER_HEADER_DATAGRAM) {
                if (host == QHostAddress::LocalHost) {
                    continue;
                }
                QNetworkDatagram _datagram(PROVIDER_HEADER_DATAGRAM.toLocal8Bit(), host, port);
                mUdpFinderSocket->writeDatagram(_datagram);
                qDebug() << "Client Addr:" << host.toString() << port;
            }
        }
    }
}

