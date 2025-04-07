#include "amqpCommon.h"

#include <fmt/format.h>

namespace amqpCommon {



    void declareQueue(AMQP::Channel &channel,
                      const std::string &queue1,
                      const std::string &exchange1) {
        channel.declareQueue(queue1)
                .onSuccess([queue1]() {
                    std::cout << fmt::format("Queue \"{}\" declared\n", queue1);
                })
                .onError([](const char *msg) {
                    std::cerr << "Queue error: " << msg << "\n";
                });

        channel.bindQueue(exchange1, queue1, queue1);
    }

    void amqpConsumerService::connect() {
        auto messageCb =
                [this](const AMQP::Message &message,
                       uint64_t deliveryTag,
                       bool redelivered) {

                    std::cout << "Body: " << std::string(message.body(), message.bodySize()) << '\n';
                    std::cout << "Priority: " << (int) message.priority() << '\n';
                    std::cout << "Persistent: " << message.persistent() << '\n';
                    std::cout << "Content-Type: " << message.contentType() << '\n';
                    std::cout << "Timestamp: " << message.timestamp() << '\n';
                    for (const auto &key: message.headers().keys()) {
                        std::cout << "Header [" << key << "] = " << message.headers().operator[](key)<<'\t'<< message.headers().operator[](key).typeID() << '\n';//typeId
                    }
                    std::cout<<'\n';
                    m_channel.ack(deliveryTag);
                };


        auto startCb = [](const std::string &consumertag) {
            std::cout << "Consumption started successfully with consumer tag: " << consumertag << '\n';
        };


        auto errorCb = [](const char *message) {
            std::cout << "Consumption error: " << message << '\n';
        };


        m_channel.consume(m_queue)
                .onReceived(messageCb)
                .onSuccess(startCb)
                .onError(errorCb);


        m_serviceThread = std::thread([this]() { m_service.run(); });
    }

    void amqpConsumerService::disconnect() {


        m_service.post([this]() {
            m_connection.close();
        });


        if (m_serviceThread.joinable()) {
            m_serviceThread.join();
        }
    }

    amqpConsumerService::~amqpConsumerService() {
        disconnect();
    }

    amqpConsumerService::amqpConsumerService(const std::string &connectionString,
                                             const std::string &queueName) :
            m_service(1),
            m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
            m_handler(m_service, m_work),
            m_connection(&m_handler, AMQP::Address(connectionString)),
            m_channel(&m_connection),
            m_queue(queueName) {

        m_channel.onError([](const char *message) {
            std::cout << "Channel error: " << message << '\n';
        });

    }

    MyHandler::MyHandler(boost::asio::io_service &service,
                         std::unique_ptr<boost::asio::io_service::work> &workRef) :
            AMQP::LibBoostAsioHandler(service),
            m_work(workRef) {}

    amqpPublisherService::amqpPublisherService(const std::string &connectionString,
                                               const std::vector<std::string> &queues)
                                               :
            m_service(1),
            m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
            m_handler(m_service, m_work),
            m_connection(&m_handler, AMQP::Address(connectionString)),
            m_channel(&m_connection),
            m_queues(queues) {

        m_channel.onError([](const char *message) {
            std::cout << fmt::format("Channel error: {}\n", message);
        });

        m_channel.declareExchange(defaultExhc,AMQP::direct).onSuccess(
                [] {
                    std::cout << fmt::format("Exchange \"{}\" declared\n", defaultExhc);
                }).onError([](const char *msg) {
            std::cerr << "Exchange error: " << msg << "\n";
        });

        for (const auto &q: m_queues) {
            std::cout<<m_queues.size()<<'\n';
            declareQueue(m_channel,q,q);
        }

    }

    void amqpPublisherService::removeQueue(size_t i) {
        if (i >= m_queues.size()) {
            throw shared::outOfRange(i,0,m_queues.size()-1);
        }
        m_queues.erase(m_queues.begin() + i);

    }

    void amqpPublisherService::addQueue(const std::string &queue, bool create) {
        if (create) {
           declareQueue(m_channel, queue, defaultExhc);
        }
        m_queues.push_back(queue);
    }

    amqpPublisherService::~amqpPublisherService() {
        endLoop();
        m_queues.clear();
    }

    void amqpPublisherService::publish(EnvelopePtr message, size_t i) {
        if (i >= m_queues.size()) {
            throw shared::outOfRange(i,0,m_queues.size()-1);
        }
        message->setTimestamp(std::chrono::high_resolution_clock().now().time_since_epoch().count());
        m_channel.publish(defaultExhc, m_queues[i], *message);
        //todo try https://github.com/CopernicaMarketingSoftware/AMQP-CPP?tab=readme-ov-file#publisher-confirms
    }

    void amqpPublisherService::endLoop() {
        if (m_work) {
            m_work.reset();
        }

        m_service.post([this]() {
            m_connection.close();
        });


        if (m_serviceThread.joinable()) {
            m_serviceThread.join();
        }
        m_service.reset();

    }

    void amqpPublisherService::restartLoop() {
        if (m_serviceThread.joinable()) {
            m_serviceThread.join();
        }
        if (!m_work) {
            m_work = std::make_unique<boost::asio::io_service::work>(m_service);
        }
        m_serviceThread = std::thread([this]() {
            m_service.run();
        });
    }
}