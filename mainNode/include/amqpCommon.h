#pragma once



#include <string>
#include <vector>
#include <unordered_map>
#include <thread>

#include <fmt/format.h>


#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <amqpcpp/libboostasio.h>
#include <json/json.h>
namespace amqpCommon
{
    static inline std::string exchange="testexch";
    static inline std::string queue="testqueue";



    static inline  std::string cString = "amqp://sysadmin:syspassword@localhost/";

    void declareExchange(AMQP::Channel& channel,
                         const std::string & exchange1);

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



    void deleteQueue(AMQP::Channel& channel,
                     const std::string& queueName,
                     bool ifUnused = false,
                     bool ifEmpty = false);

    void deleteExchange(AMQP::Channel& channel,
                        const std::string& exchangeName,
                        bool ifUnused = false);


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

        void publish(AMQP::Message&&message,size_t i);

        ~amqpPublisherService();



    private:
        boost::asio::io_service m_service;
        std::unique_ptr<boost::asio::io_service::work> m_work;

        std::vector<std::string> m_queues;
        AMQP::TcpChannel m_channel;


        MyHandler m_handler;
        AMQP::TcpConnection m_connection;

        std::thread m_serviceThread;
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