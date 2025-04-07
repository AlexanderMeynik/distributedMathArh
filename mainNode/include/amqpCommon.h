#pragma once

#ifndef DISTRIBUTED_MATH_ARH_AMQPRESTCOMMON_H
#define DISTRIBUTED_MATH_ARH_AMQPRESTCOMMON_H

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>

#include <boost/asio/io_service.hpp>
#include <amqpcpp/libboostasio.h>



namespace amqpCommon
{
    static inline std::string exchange="testexch";
    static inline std::string queue="testqueue";



    static inline  std::string cString = "amqp://sysadmin:syspassword@localhost/";

    using AMQP::Envelope;
    using  EnvelopePtr=std::shared_ptr<Envelope>;


    void  declareQueue(AMQP::Channel& channel,
                       const std::string & queue1,
                       const std::string & exchange1);


    void sendMessage(AMQP::Channel& channel,
                     std::string_view exchange1,
                     std::string_view  routingKey,
                     std::string_view  body,
                     bool persistent = true);

    void consumeMessages(AMQP::Channel& channel,
                         std::string_view  queue1);

    class MyHandler : public AMQP::LibBoostAsioHandler {
    public:
        MyHandler(boost::asio::io_service& service,
                  std::unique_ptr<boost::asio::io_service::work>& workRef);

        void onClosed(AMQP::TcpConnection* connection) override {
            m_work.reset();

            std::cout << "Connection closed.\n";
        }

        void onError(AMQP::TcpConnection* connection,
                     const char* message) override {
            std::cout << "Connection error: " << message << '\n';
        }

        void onConnected(AMQP::TcpConnection* connection) override {
            std::cout << "Connection established successfully." << '\n';
        }
    private:

        std::unique_ptr<boost::asio::io_service::work>& m_work;
    };

    class amqpPublisherService
    {
    public:

        amqpPublisherService(const std::string& connectionString,
                            const std::vector<std::string>&queues={});

        void removeQueue(size_t i);
        void addQueue(const std::string &queue1,
                      bool create);

        void publish( EnvelopePtr message, size_t i);

        void endLoop();

        void restartLoop();

        ~amqpPublisherService();



    private:
        boost::asio::io_service m_service;
        std::unique_ptr<boost::asio::io_service::work> m_work;

        MyHandler m_handler;
        AMQP::TcpConnection m_connection;
        AMQP::TcpChannel m_channel;
        std::vector<std::string> m_queues;
        std::thread m_serviceThread;

        static inline const std::string defaultExhc="testexch";//todo delete

    };

    class amqpConsumerService {
    public:
        amqpConsumerService(const std::string& connectionString,
                            const std::string& queueName);

        void connect();

        void disconnect();

        ~amqpConsumerService();

    private:
        boost::asio::io_service m_service;
        std::unique_ptr<boost::asio::io_service::work> m_work;

        MyHandler m_handler;
        AMQP::TcpConnection m_connection;
        AMQP::TcpChannel m_channel;

        std::string m_queue;
        std::thread m_serviceThread;
    };

}

#endif //DISTRIBUTED_MATH_ARH_AMQPRESTCOMMON_H