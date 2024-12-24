
#ifndef MAINTAINING_API_H
#define MAINTAINING_API_H

#include <QObject>
#include <QTimer>

#include <functional>
#include <thread>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include "proto/api.pb.h"
#include "proto/api.grpc.pb.h"

/**
 *  N.B. для экономии места и времени опеределим в этом модуле три класса:
 *  MaintainingApiService, ServerBuider, ScopedThread
 */

using RestartTimer = std::function<void()>;
using SendLogMessage = std::function<void(std::string)>;

class MaintainingApiService final : public MaintainingApi::Service
{
    ::grpc::Status Ping(::grpc::ServerContext* context, const PingRequest* request,
                  PingResponse* reply) override
    {
        restartTimer();
        sendLogMessage(request->clientip());

        return grpc::Status::OK;
    }

public:
    RestartTimer restartTimer;
    SendLogMessage sendLogMessage;

    void setRestartTimer(RestartTimer f)
    {
        restartTimer = f;
    }

    void setSendLogMessage(SendLogMessage f)
    {
        sendLogMessage = f;
    }
};

class ServerBuilder : public QObject
{
    Q_OBJECT

    QTimer *timer;
    std::string server_address;
    grpc::ServerBuilder builder;

public:
    ServerBuilder()
    {
        timer = new QTimer(this);

        connect(timer, &QTimer::timeout, this, &ServerBuilder::stopTimer);
        connect(this, &ServerBuilder::start, this, &ServerBuilder::startTimer);
    }

    void set(const std::string ip, uint16_t port)
    {
            server_address =
                    ip +
                    std::string(":") +
                    std::to_string(port);
    }

    void startLoop()
    {
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address,
                                 grpc::InsecureServerCredentials());

        MaintainingApiService service;
        grpc::EnableDefaultHealthCheckService(true);

        RestartTimer f1 = std::bind(&ServerBuilder::restartTimer, this);
        service.setRestartTimer(f1);

        SendLogMessage f2 = std::bind(&ServerBuilder::sendLogMessage, this, std::placeholders::_1);
        service.setSendLogMessage(f2);

        builder.RegisterService(&service);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

        server->Wait();
    }

    void restartTimer()
    {
        emit stopUdpBroadcast();
        emit start();
    }

private slots:
    void stopTimer()
    {
        timer->stop();
        emit startUdpBroadcast();
    }

    void startTimer()
    {
         timer->start(15000);
    }

    void sendLogMessage(std::string msg)
    {
        emit logMessage(QString("Ping: Client ip: ") +
                        QString(msg.c_str()));
    }

signals:
    void start();
    void logMessage(const QString& msg);

    void stopUdpBroadcast();
    void startUdpBroadcast();
};

class ScopedThread
{
private:
  std::thread thread;

public:
  ScopedThread(std::thread t) : thread(std::move(t))
  {
     if (!thread.joinable())
       throw std::logic_error("No thread");
  }

  ~ScopedThread()
  {
     thread.detach();
  }

  ScopedThread(const ScopedThread &) = delete;
  ScopedThread& operator=(const ScopedThread &) = delete;
};

#endif // MAINTAINING_API_H
