#include "amqpCommon.h"

namespace amqpCommon {

    void deleteQueue(AMQP::Channel &channel, const std::string &queueName, bool ifUnused, bool ifEmpty) {
        channel.removeQueue(queueName,
                            (ifUnused ? AMQP::ifunused : AMQP::direct) |
                            (ifEmpty ? AMQP::ifempty : AMQP::direct)
                )
                .onSuccess([&queueName]() {
                    std::cout << fmt::format("Queue \"{}\" deleted\n", queueName);
                })
                .onError([](const char *error) {
                    std::cerr << "Queue deletion error: " << error << "\n";
                });
    }

    void deleteExchange(AMQP::Channel &channel, const std::string &exchangeName, bool ifUnused) {
        channel.removeExchange(exchangeName,
                               ifUnused ? AMQP::ifunused : AMQP::direct
                )
                .onSuccess([&exchangeName]() {
                    std::cout << fmt::format("Exchange \"{}\" deleted\n", exchangeName);
                })
                .onError([](const char *error) {
                    std::cerr << "Exchange deletion error: " << error << "\n";
                });
    }

    void consumeMessages(AMQP::Channel &channel, std::string_view queue1) {
        channel.consume(queue1)
                .onReceived([&](const AMQP::Message &msg, uint64_t tag, bool redelivered) {


                    std::string body(msg.body(), msg.bodySize());
                    std::cout << "Received: " << body << "\n";
                    for (int i = 0; i < 10; ++i) {
                        std::cout << i << ((i < 9) ? '\t' : '\n');
                    }


                    channel.ack(tag);
                });
    }

    void
    sendMessage(AMQP::Channel &channel,
                std::string_view exchange1,
                std::string_view routingKey,
                std::string_view body,
                bool persistent) {

        channel.publish(exchange1, routingKey, body);
    }

    void declareQueue(AMQP::Channel &channel,
                      const std::string &queue1,
                      const std::string &exchange1) {
        channel.declareQueue(queue1)
                .onSuccess([&queue1]() {
                    std::cout << fmt::format("Queue \"{}\" declared\n", queue1);
                })
                .onError([](const char *msg) {
                    std::cerr << "Queue error: " << msg << "\n";
                });

        channel.bindQueue(exchange1, queue1, queue1);//todo binding keys for queues
    }

    void declareExchange(AMQP::Channel &channel,
                         const std::string &exchange1) {
        channel.declareExchange(exchange1, AMQP::direct)
                .onSuccess([&exchange1]() {
                    std::cout << fmt::format("Exchange \"{}\" declared\n", exchange1);
                })
                .onError([](const char *msg) {
                    std::cerr << "Exchange error: " << msg << "\n";
                });
    }

    void amqpConsumerService::connect() {
        auto messageCb =
                [this](const AMQP::Message &message,
                       uint64_t deliveryTag,
                       bool redelivered) {
                    /*Json::Value val;
                    Json::Reader reader;
                    if (reader.parse(message.body(), message.body() + message.bodySize(), val)) {
                        std::cout << "Message received: size = " << message.bodySize()
                                  << ", content = " << val.toStyledString() << '\n';
                    } else {
                        std::cout << "Failed to parse message as JSON." << '\n';
                    }*/

                    std::cout << "Body: " << std::string(message.body(), message.bodySize()) << '\n';
                    std::cout << "Priority: " << (int) message.priority() << '\n';
                    std::cout << "Persistent: " << message.persistent() << '\n';
                    std::cout << "Content-Type: " << message.contentType() << '\n';
                    std::cout << "Timestamp: " << message.timestamp() << '\n';
                    for (const auto &key: message.headers().keys()) {
                        std::cout << "Header [" << key << "] = " << message.headers().operator[](key) << '\n';//typeId
                    }


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
                                               const std::vector<std::string> &queues) :
            m_service(1),
            m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
            m_handler(m_service, m_work),
            m_connection(&m_handler, AMQP::Address(connectionString)),
            m_channel(&m_connection),
            m_queues(queues) {

        m_channel.onError([](const char *message) {
            std::cout << fmt::format("Channel error: {}\n",message);
        });
        declareExchange(m_channel,exchange);
        for (size_t i = 0; i < queues.size(); ++i) {
            declareQueue(m_channel,queue,exchange);
        }

        m_serviceThread = std::thread([this]() { m_service.run(); });
        
        
    }

    void amqpPublisherService::removeQueue(size_t i) {
        if(i>=m_queues.size())
        {
            throw std::out_of_range(fmt::format("Index {} is larger than struct size {}",i,m_queues.size()));
        }
        m_queues.erase(m_queues.begin()+i);

    }

    void amqpPublisherService::addQueue(const std::string &queue1, bool create) {
        declareQueue(m_channel,queue1,exchange);
        m_queues.push_back(queue1);
    }

    amqpPublisherService::~amqpPublisherService() {
        m_queues.clear();
        m_work.reset();

        if(m_serviceThread.joinable())
        {
            m_serviceThread.join();
        }
    }

    void amqpPublisherService::publish(AMQP::Message &&message, size_t i) {
        if(i>=m_queues.size())
        {
            throw std::out_of_range(fmt::format("Index {} is larger than struct size {}",i,m_queues.size()));
        }
        m_channel.publish(exchange,m_queues[i],message);

    }
}