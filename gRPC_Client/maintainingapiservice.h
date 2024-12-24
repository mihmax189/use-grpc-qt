#ifndef MAINTAINING_API_H
#define MAINTAINING_API_H

#include <unordered_map>
#include <atomic>
#include <condition_variable>
#include <chrono>

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include "proto/api.pb.h"
#include "proto/api.grpc.pb.h"

class Client
{
    std::mutex mtx_exit;
    std::mutex mtx_ping;
    std::condition_variable cv_ping;

    bool exit = false;
    bool ping = false;

    std::string addr;

public:
    Client(std::string ip, uint16_t port)
    {
        addr = ip + std::string(":") + std::to_string(port);
        createThread(addr);
    }

    ~Client()
    {
        Exit();
    }

    void Exit()
    {
        // если поток заблокирован в ожидании поднятия ping, поднимаем разблокируем его
        // и поднимаем флаг exit для выхода из цикла.
        Ping(true);

        std::lock_guard<std::mutex> lock_mtx_exit(mtx_exit);
        exit = true;
    }

    void Ping(bool p)
    {
        std::lock_guard<std::mutex> lock_mtx_ping(mtx_ping);
        ping = p;
        cv_ping.notify_one();
    }

private:
    void sendPing(const std::string &addr_)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock_mtx_exit(mtx_exit);
            if (exit)
                break;
            lock_mtx_exit.unlock();

            std::unique_lock<std::mutex> lock_mtx_ping(mtx_ping);
            cv_ping.wait(lock_mtx_ping, [this]
                {
                    return this->ping;
                });
            lock_mtx_ping.unlock();

            // added addr.isempty()!
            PingRequest request;
            PingResponse response;

            request.set_clientip(addr);
            auto channel = grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());

            std::unique_ptr<MaintainingApi::Stub> stub = MaintainingApi::NewStub(channel);
            grpc::ClientContext context;
            grpc::Status status = stub->Ping(&context, request, &response);

            if (!status.ok())
            {
                std::cerr << "RPC failed: " << status.error_code() << ": "
                          << status.error_message() << std::endl;
            }

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(5000ms);
        }
    }

public:
    void createThread(const std::string &addr_)
    {
        std::thread t(&Client::sendPing, this, addr_);
        t.detach();
    }
};

#endif // MAINTAINING_API_H
