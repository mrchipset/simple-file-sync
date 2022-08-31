#ifndef SYNCSERVICEFINDER_H
#define SYNCSERVICEFINDER_H

#include <QObject>
#include <QUdpSocket>
#include <QMap>

#include "SyncerRunnable.h"

class SyncServiceFinder : public QObject, public SyncerRunnable
{
    Q_OBJECT
public:
    typedef struct{
        QHostAddress Host;
        uint16_t Port;
    } EndPoint;

public:
    SyncServiceFinder(QObject* parent);
    ~SyncServiceFinder();

    void setPortRange(uint16_t lower, uint16_t upper);
    void getPortRange(uint16_t& lower, uint16_t& upper);

    void executeSearch();
    bool isFind();
    QVector<EndPoint> getFoundEndPoints() const;
Q_SIGNALS:
    void searchFinished();
private:
    uint16_t mLowerPort;
    uint16_t mUpperPort;
    QMap<quint32, EndPoint> mEndPoints;
    void readPendingDatagram(QUdpSocket* socket);

protected:
    virtual void worker() override;
};

#endif // SYNCSERVICEFINDER_H
