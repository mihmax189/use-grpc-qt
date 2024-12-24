#ifndef GRPCSERVER_H
#define GRPCSERVER_H

#include <QHash>
#include <QTime>

class gRpcServerParams
{
    QString ip;
    uint port;
    bool status;           // true - is online, false - is offline
    QTime lastPingTime;

public:
    gRpcServerParams(const QString& ip, uint port)
    {
        this->ip = ip;
        this->port = port;
        this->status = false;
        this->lastPingTime = QTime();
    }

    void setStatus(bool s) { status = s; }
    bool getStatus() { return status; }
    const QString& getIp() const{ return ip; }
    uint getPort() const { return port; }
    void setLastPingTime(QTime t) { lastPingTime = t; }
    QTime getLastPingTime() { return lastPingTime; }
};

inline bool operator==(const gRpcServerParams& first, const gRpcServerParams& second)
{
    return (first.getIp() == second.getIp() &&
            first.getPort() == second.getPort());
}

inline uint qHash(const gRpcServerParams &server)
{
    return qHash(server.getIp()) ^ qHash(server.getPort());
}

#endif // GRPCSERVER_H
