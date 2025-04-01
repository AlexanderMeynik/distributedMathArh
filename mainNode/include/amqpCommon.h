#pragma once



#include <string>


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
    constexpr static inline const char * exchange="testexch";
    constexpr static inline const char * queue="testqueue";


    auto inline startCb = [](const std::string &consumertag) {

        std::cout << "consume operation started" << std::endl;
    };

    auto inline errorCb = [](const char *message) {

        std::cout << "consume operation failed" << std::endl;
    };


    auto inline cancelledCb = [](const std::string &consumertag) {

        std::cout << "consume operation cancelled by the RabbitMQ server" << std::endl;
    };




    constexpr static inline  const char * adress = "amqp://sysadmin:syspassword@localhost/";

    void declareExchange(AMQP::Channel& channel, std::string_view exchange1);

    void  declareQueue(AMQP::Channel& channel, std::string_view queue1,std::string_view exchange1);


    void sendMessage(AMQP::Channel& channel, std::string_view exchange1, std::string_view  routing_key,
                      std::string_view  body, bool persistent = true);

    void consumeMessages(AMQP::Channel& channel, std::string_view  queue1);



    void deleteQueue(AMQP::Channel& channel, const std::string& queue_name, bool ifUnused = false, bool ifEmpty = false);

    void deleteExchange(AMQP::Channel& channel, const std::string& exchange_name, bool ifUnused = false);

}