



#include "amqpCommon.h"

#include <json/json.h>
#include <iomanip>
//todo run without boost as event loop
//create and remove exchanges an queues
//retrieving queues and their names
//conenction create/remove
using namespace amqpCommon;
int main(int argc,char * argv[])
{
    int a=std::stol(argv[1]);


    boost::asio::io_service service(1);


    AMQP::LibBoostAsioHandler handler(service);

    // make a connection
    AMQP::TcpConnection connection(&handler, AMQP::Address(adress));

    // we need a channel too
    AMQP::TcpChannel channel(&connection);


    declareExchange(channel,exchange);

    declareQueue(channel,queue,exchange);

    for (int i = 0; i < a; ++i) {
        Json::Value message;
        message["number"]=i;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::stringstream ss;
        ss<<std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        message["timestamp"]= ss.str();
        //todo try https://github.com/CopernicaMarketingSoftware/AMQP-CPP?tab=readme-ov-file#publisher-confirms
        channel.publish(exchange,queue,message.toStyledString());

        std::cout<<i<<'\n';

    }

    service.run();

    return 0;
}