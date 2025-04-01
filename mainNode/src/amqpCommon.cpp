#include "amqpCommon.h"

namespace amqpCommon
{

    void deleteQueue(AMQP::Channel &channel, const std::string &queueName, bool ifUnused, bool ifEmpty) {
        channel.removeQueue(queueName,
                            (ifUnused ? AMQP::ifunused : AMQP::direct) |
                            (ifEmpty ? AMQP::ifempty : AMQP::direct)
                )
                .onSuccess([&queueName]() {
                    std::cout << fmt::format("Queue \"{}\" deleted\n", queueName);
                })
                .onError([](const char* error) {
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
                .onError([](const char* error) {
                    std::cerr << "Exchange deletion error: " << error << "\n";
                });
    }

    void consumeMessages(AMQP::Channel &channel, std::string_view queue1) {
        channel.consume(queue1)
                .onReceived([&](const AMQP::Message& msg, uint64_t tag, bool redelivered) {


                    std::string body(msg.body(), msg.bodySize());
                    std::cout << "Received: " << body << "\n";
                    for (int i = 0; i < 10; ++i) {
                        std::cout<<i<<((i<9)?'\t':'\n');
                    }


                    channel.ack(tag);
                });
    }

    void
    sendMessage(AMQP::Channel &channel, std::string_view exchange1, std::string_view routingKey, std::string_view body,
                bool persistent) {

        channel.publish(exchange1, routingKey, body);
    }

    void declareQueue(AMQP::Channel &channel, const std::string & queue1, const std::string & exchange1) {
        channel.declareQueue(queue1)
                .onSuccess([&queue1]() {
                    std::cout <<fmt::format("Queue \"{}\" declared\n",queue1);
                })
                .onError([](const char* msg) {
                    std::cerr << "Queue error: " << msg << "\n";
                });

        channel.bindQueue(exchange1, queue1, queue1);//todo binding keys for queues
    }

    void declareExchange(AMQP::Channel &channel, const std::string & exchange1) {
        channel.declareExchange(exchange1, AMQP::direct)
                .onSuccess([&exchange1]() {
                    std::cout << fmt::format("Exchange \"{}\" declared\n",exchange1);
                })
                .onError([](const char* msg) {
                    std::cerr << "Exchange error: " << msg << "\n";
                });
    }
}