#include "logentryblockedip.h"

LogEntryBlockedIp::LogEntryBlockedIp(quint8 blockReason, quint8 ipProto, quint16 localPort,
        quint16 remotePort, quint32 localIp, quint32 remoteIp, quint32 pid,
        const QString &kernelPath) :
    LogEntryBlocked(pid, kernelPath),
    m_blockReason(blockReason),
    m_ipProto(ipProto),
    m_localPort(localPort),
    m_remotePort(remotePort),
    m_localIp(localIp),
    m_remoteIp(remoteIp)
{
}

void LogEntryBlockedIp::setInbound(bool inbound)
{
    m_inbound = inbound;
}

void LogEntryBlockedIp::setBlockReason(quint8 blockReason)
{
    m_blockReason = blockReason;
}

void LogEntryBlockedIp::setIpProto(quint8 proto)
{
    m_ipProto = proto;
}

void LogEntryBlockedIp::setLocalPort(quint16 port)
{
    m_localPort = port;
}

void LogEntryBlockedIp::setRemotePort(quint16 port)
{
    m_remotePort = port;
}

void LogEntryBlockedIp::setLocalIp(quint32 ip)
{
    m_localIp = ip;
}

void LogEntryBlockedIp::setRemoteIp(quint32 ip)
{
    m_remoteIp = ip;
}

QString LogEntryBlockedIp::reasonToString(quint8 blockReason)
{
    switch (blockReason) {
    case ReasonIpInet:
        return QObject::tr("Not allowed Internet address");
    case ReasonReauth:
        return QObject::tr("Old connection closed on startup");
    case ReasonProgram:
        return QObject::tr("Programs logic");
    case ReasonAppGroupFound:
        return QObject::tr("App. Group logic");
    case ReasonAppGroupDefault:
        return QObject::tr("App. Group default logic");
    default:
        return QObject::tr("Unknown");
    }
}
