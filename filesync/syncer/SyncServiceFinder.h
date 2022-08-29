#ifndef SYNCSERVICEFINDER_H
#define SYNCSERVICEFINDER_H

#include "AvailablePortHelper.h"

class SyncServiceFinder
{
public:
    SyncServiceFinder();

    void setPortRange(uint16_t lower, uint16_t upper);
    void getPortRange(uint16_t& lower, uint16_t& upper);

    void executeSearch();
    bool isFind();

private:
    typedef struct{
        QString Addr;
        uint16_t Port;
    } EndPoint;

    uint16_t mLowerPort;
    uint16_t mUpperPort;
    QVector<EndPoint> mEndPoints;
};

#endif // SYNCSERVICEFINDER_H
