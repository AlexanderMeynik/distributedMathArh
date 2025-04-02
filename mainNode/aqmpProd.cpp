

//retrieving queues and their names
//conenction create/remove

#include "amqpCommon.h"

#include <json/json.h>
#include <iomanip>

using namespace amqpCommon;
int main(int argc,char * argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_messages>\n";
        return 1;
    }
    int numMessages;
    try {
        numMessages = std::stoi(argv[1]);
        if (numMessages <= 0) {
            throw std::invalid_argument("Number of messages must be positive.");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid number of messages - " << e.what() << '\n';
        return 1;
    }


    boost::asio::io_service service(1);
    AMQP::LibBoostAsioHandler handler(service);

    AMQP::TcpConnection connection(&handler, AMQP::Address(cString));
    AMQP::TcpChannel channel(&connection);\

    channel.onError([](const char* message) {
        std::cerr << "Channel error: " << message << '\n';
    });

    declareExchange(channel,exchange);

    declareQueue(channel,queue,exchange);

    for (int i = 0; i < numMessages; ++i) {
        Json::Value message;
        message["number"]=i;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::stringstream ss;
        ss<<std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        message["timestamp"]= ss.str();


        channel.publish(exchange,queue,message.toStyledString());//todo message persistency, priority ...

        std::cout<<i<<'\n';



    }

    service.run();

    std::cout << "All messages published successfully."<<'\n';



    return 0;
}

//todo try https://github.com/CopernicaMarketingSoftware/AMQP-CPP?tab=readme-ov-file#publisher-confirms