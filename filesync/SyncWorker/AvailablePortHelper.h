#ifndef AVAILABLEPORTHELPER_H
#define AVAILABLEPORTHELPER_H


#include "SyncWorker_global.h"
#include <cstdint>
#include <QVector>

class SYNCWORKER_EXPORT AvailablePortHelper
{
public:
    AvailablePortHelper();

    static bool GetAllTcpConnectionPorts(QVector<uint16_t>& ports);
    static bool GetAllUdpConnectionPorts(QVector<uint16_t>& ports);

    static bool GetAvailableTcpPorts(uint16_t portLower, uint16_t portUpper, QVector<uint16_t>& ports);
    static bool GetAvailableUdpPorts(uint16_t portLower, uint16_t portUpper, QVector<uint16_t>& ports);

};

#endif // AVAILABLEPORTHELPER_H
