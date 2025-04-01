



#include "amqpCommon.h"

#include <json/json.h>
#include <iomanip>
//todo run without boost as event loop
//create and remove exchanges an queues
//retrieving queues and their names
//conenction create/remove
using namespace amqpCommon;
int main()
{

    // access to the boost asio handler
    // note: we suggest use of 2 threads - normally one is fin (we are simply demonstrating thread safety).
    boost::asio::io_service service(1);

    // handler for libev
    AMQP::LibBoostAsioHandler handler(service);

    // make a connection
    AMQP::TcpConnection connection(&handler, AMQP::Address(adress));

    // we need a channel too
    AMQP::TcpChannel channel(&connection);


    declareExchange(channel,exchange);

    declareQueue(channel,queue,exchange);
    for (int i = 0; i < 100; ++i) {
        std::string a=std::to_string(i);
        Json::Value message;
        message["number"]=i;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        //std::cout <<  << std::endl;
        std::stringstream ss;
        ss<<std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        message["timestamp"]= ss.str();
        channel.publish(exchange,queue,message.toStyledString());
    }




    return service.run();
}