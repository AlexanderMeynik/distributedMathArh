#pragma once

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "common/errorHandling.h"

#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>

#include <boost/asio/io_service.hpp>
#include <amqpcpp/libboostasio.h>


namespace amqpCommon {

    using AMQP::Envelope;
    using EnvelopePtr = std::shared_ptr<Envelope>;

    /**
     * @brief
     * @param channel
     * @param queue1
     * @param exchange1
     */
    void declareQueue(AMQP::Channel &channel,
                      const std::string &queue1,
                      const std::string &exchange1);


    /**
     * @brief
     * @param channel
     * @param exchange1
     * @param routingKey
     * @param body
     * @param persistent
     */
    void sendMessage(AMQP::Channel &channel,
                     std::string_view exchange1,
                     std::string_view routingKey,
                     std::string_view body,
                     bool persistent = true);

    /**
     * @brief
     * @param channel
     * @param queueName
     */
    void consumeMessages(AMQP::Channel &channel,
                         std::string_view queueName);

    /**
     * @brief Custom for connection events logging
     */
    class MyHandler : public AMQP::LibBoostAsioHandler {
    public:
        MyHandler(boost::asio::io_service &service,
                  std::unique_ptr<boost::asio::io_service::work> &workRef);

        void onClosed(AMQP::TcpConnection *connection) override {
            m_work.reset();

            std::cout << "Connection closed.\n";
        }

        void onError(AMQP::TcpConnection *connection,
                     const char *message) override {
            std::cout << "Connection error: " << message << '\n';
        }

        void onConnected(AMQP::TcpConnection *connection) override {
            std::cout << "Connection established successfully." << '\n';
        }

    private:
        std::unique_ptr<boost::asio::io_service::work> &m_work;
    };

    /**
     * @brief Service that allow to publish messages to multiple queues
     */
    class amqpPublisherService {
    public:

        /**
         * @details Will declare all queues in provide array
         * @param connectionString
         * @param queues
         */
        amqpPublisherService(const std::string &connectionString,
                             const std::vector<std::string> &queues = {});

        /**
         * @brief delted i's queue
         * @param i
         * @throws shared::outOfRange
         */
        void removeQueue(size_t i);

        /**
         * Adds new queue
         * @param queue
         * @param create
         */
        void addQueue(const std::string &queue,
                      bool create);

        /**
         * @brief publishe message to i's queue
         * @param message
         * @param i
         * @throws shared::outOfRange
         */
        void publish(EnvelopePtr message, size_t i);


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

        static inline const std::string defaultExhc = "testexch";//todo delete

    };

    /**
     * @brief Service that allow to start and end reading event loop from specified queue
     */
    class amqpConsumerService {
    public:
        /**
         *
         * @param connectionString
         * @param queueName
         */
        amqpConsumerService(const std::string &connectionString,
                            const std::string &queueName);

        /**
         * @brief Starts event loop to read from queue1
         */
        void connect();

        /**
         * @brief Ends event loop()
         */
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
