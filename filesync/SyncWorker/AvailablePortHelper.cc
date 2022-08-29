#include "AvailablePortHelper.h"
#include <Windows.h>
#include <WinSock.h>
#include <tcpmib.h>
#include <iphlpapi.h>

#include <memory>
#include <numeric>

#include <QSet>

AvailablePortHelper::AvailablePortHelper()
{

}

bool AvailablePortHelper::GetAllTcpConnectionPorts(QVector<uint16_t> &ports)
{
    ULONG size = 0;
    GetTcpTable(NULL, &size, TRUE);
    std::unique_ptr<char> buffer(new char[size]);

    QSet<uint16_t> tmpPort;
    PMIB_TCPTABLE tcpTable = reinterpret_cast<PMIB_TCPTABLE>(buffer.get());
    if (GetTcpTable(tcpTable, &size, FALSE) == NO_ERROR) {
        for (uint32_t i = 0; i < tcpTable->dwNumEntries; ++i) {
            tmpPort.insert(ntohs(static_cast<uint16_t>(tcpTable->table[i].dwLocalPort)));
        }
    }

    ports.clear();
    for (auto iter=tmpPort.begin();iter!=tmpPort.end();++iter) {
        ports.append(*iter);
    }
    std::sort(ports.begin(), ports.end());
    return true;
}

bool AvailablePortHelper::GetAllUdpConnectionPorts(QVector<uint16_t> &ports)
{
    ULONG size = 0;
    GetUdpTable(NULL, &size, TRUE);
    std::unique_ptr<char> buffer(new char[size]);

    QSet<uint16_t> tmpPort;
    PMIB_UDPTABLE udpTable = reinterpret_cast<PMIB_UDPTABLE>(buffer.get());
    if (GetUdpTable(udpTable, &size, FALSE) == NO_ERROR) {
        for (uint32_t i = 0; i < udpTable->dwNumEntries; ++i) {
            tmpPort.insert(ntohs(static_cast<uint16_t>(udpTable->table[i].dwLocalPort)));
        }
    }

    ports.clear();
    for (auto iter=tmpPort.begin();iter!=tmpPort.end();++iter) {
        ports.append(*iter);
    }
    std::sort(ports.begin(), ports.end());
    return true;
}

bool AvailablePortHelper::GetAvailableTcpPorts(uint16_t portLower, uint16_t portUpper, QVector<uint16_t> &ports)
{
    ports.resize(portUpper - portLower + 1);
    for (uint16_t i = 0; i < ports.size(); ++i) {
        ports[i] = i + portLower;
    }

    QVector<uint16_t> usedPorts;
    GetAllTcpConnectionPorts(usedPorts);
    for (auto& port : usedPorts) {
        ports.removeOne(port);
    }
    return true;
}

bool AvailablePortHelper::GetAvailableUdpPorts(uint16_t portLower, uint16_t portUpper, QVector<uint16_t> &ports)
{
    ports.resize(portUpper - portLower + 1);
    for (uint16_t i = 0; i < ports.size(); ++i) {
        ports[i] = i + portLower;
    }

    QVector<uint16_t> usedPorts;
    GetAllUdpConnectionPorts(usedPorts);
    for (auto& port : usedPorts) {
        ports.removeOne(port);
    }
    return true;
}

