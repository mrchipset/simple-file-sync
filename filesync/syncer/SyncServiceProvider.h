#ifndef SYNCSERVICEPROVIDER_H
#define SYNCSERVICEPROVIDER_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>

class SyncServiceProvider : public QObject
{
    Q_OBJECT
public:
    explicit SyncServiceProvider(QObject *parent = nullptr);
    bool createService();

private:
    QUdpSocket* mUdpFinderSocket;
    QTcpServer* mTcpServer;

private Q_SLOTS:
    void onDatagramPending();
};

#endif // SYNCSERVICEPROVIDER_H
