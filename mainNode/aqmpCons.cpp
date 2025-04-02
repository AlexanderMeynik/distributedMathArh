
#include "amqpCommon.h"
#include <thread>


//todo queue http service(0curl,qt?)
//todo erro handling(can i throw exception from callbacks)_ how can i notify the main programm


class amqpConsumerService {
public:
    amqpConsumerService(const std::string& connectionString, const std::string& queueName)
            : m_service(1),
              m_work(std::make_unique<boost::asio::io_service::work>(m_service)),
              m_handler(m_service, m_work),
              m_connection(&m_handler, AMQP::Address(connectionString)),
              m_channel(&m_connection),
              m_queue(queueName) {


        m_channel.onError([](const char* message) {
            std::cout << "Channel error: " << message << std::endl;
        });
    }

    void connect() {

        auto messageCb = [this](const AMQP::Message& message, uint64_t deliveryTag, bool redelivered) {
            /*Json::Value val;
            Json::Reader reader;
            if (reader.parse(message.body(), message.body() + message.bodySize(), val)) {
                std::cout << "Message received: size = " << message.bodySize()
                          << ", content = " << val.toStyledString() << std::endl;
            } else {
                std::cout << "Failed to parse message as JSON." << std::endl;
            }*/

            std::cout << "Body: " << std::string(message.body(), message.bodySize()) << '\n';
            std::cout << "Priority: " << (int)message.priority() << '\n';
            std::cout<< "Persistent: "<<message.persistent()<<'\n';
            std::cout << "Content-Type: " << message.contentType() << '\n';
            std::cout << "Timestamp: " << message.timestamp() << '\n';
            for (const auto& key : message.headers().keys()) {
                std::cout << "Header [" << key << "] = " << message.headers().operator[](key) << '\n';//typeId
            }



            m_channel.ack(deliveryTag);
        };


        auto startCb = [](const std::string& consumertag) {
            std::cout << "Consumption started successfully with consumer tag: " << consumertag << std::endl;
        };


        auto errorCb = [](const char* message) {
            std::cout << "Consumption error: " << message << std::endl;
        };


        m_channel.consume(m_queue)
                .onReceived(messageCb)
                .onSuccess(startCb)
                .onError(errorCb);


        m_serviceThread = std::thread([this]() { m_service.run(); });
    }

    void disconnect() {


        m_service.post([this]() {
            m_connection.close();
        });


        if (m_serviceThread.joinable()) {
            m_serviceThread.join();
        }
    }

    ~amqpConsumerService()
    {
        disconnect();
    }

private:
    boost::asio::io_service m_service;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    class MyHandler : public AMQP::LibBoostAsioHandler {
    public:
        MyHandler(boost::asio::io_service& service, std::unique_ptr<boost::asio::io_service::work>& workRef)
                : AMQP::LibBoostAsioHandler(service), m_work(workRef) {}

        void onClosed(AMQP::TcpConnection* connection) override {
            m_work.reset();

            std::cout << "Connection closed.\n";
        }

        void onError(AMQP::TcpConnection* connection, const char* message) override {
            std::cout << "Connection error: " << message << '\n';
        }

        void onConnected(AMQP::TcpConnection* connection) override {
            std::cout << "Connection established successfully." << '\n';
        }

    private:

        std::unique_ptr<boost::asio::io_service::work>& m_work;
    };
    MyHandler m_handler;
    AMQP::TcpConnection m_connection;
    AMQP::TcpChannel m_channel;
    std::string m_queue;
    std::thread m_serviceThread;
};



// Signal handling
static volatile std::sig_atomic_t signalReceived = 0;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        signalReceived = 1;
    }
}

int main() {

    std::signal(SIGINT, signal_handler);
    using namespace amqpCommon;

    amqpConsumerService listener(cString, queue);
    listener.connect();


    while (!signalReceived) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    listener.disconnect();
    return 0;
}
