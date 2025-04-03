

//retrieving queues and their names
//conenction create/remove

#include <iomanip>
#include "amqpCommon.h"



using namespace amqpCommon;


/*static volatile std::sig_atomic_t signalReceived = 0;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        signalReceived = 1;
    }
}

using namespace  amqpCommon;
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

    std::signal(SIGINT, signalHandler);

    amqpPublisherService service(cString);


    service.restartLoop();


    for (int i = 0; i < 3; ++i) {
        service.addQueue(fmt::format("queue{}",i),true);
    }

    for (int i = 0; i < 3; ++i) {
        std::string q=fmt::format("queue{}",i);
        for (int j = 0; j < numMessages; ++j) {


            Json::Value message;
            message["number"]=i;

            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);

            std::stringstream ss;
            ss<<std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
            message["timestamp"]= ss.str();

            auto envelope = std::make_shared<AMQP::Envelope>(message.toStyledString());
            envelope->setPersistent(true);
            envelope->setPriority(numMessages - j);
            AMQP::Table headers;
            headers["messageNum"] = j;
            headers["tt"] = ss.str();
            envelope->setHeaders(headers);

            service.publish(envelope, i);
        }
    }

    while (!signalReceived) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    service.endLoop();




    return 0;
}*/


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
    channel.declareExchange(exchange,AMQP::direct);
    //declareExchange(channel,exchange);

    std::vector<std::string> qq={"q1","q2","q3"};


    for(size_t i=0;i<qq.size();i++)
    {
        declareQueue(channel,qq[i],exchange);
    }

    for(size_t j=0;j<qq.size();j++)
    {
        for (int i = 0; i < numMessages; ++i) {
            Json::Value message;
            message["number"]=i;

            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);

            std::stringstream ss;
            ss<<std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
            message["timestamp"]= ss.str();


            channel.publish(exchange,qq[j],message.toStyledString());

            std::cout<<j<<'\t'<<i<<'\n';

        }
    }

    service.run();

    std::cout << "All messages published successfully."<<'\n';



    return 0;
}

//todo try https://github.com/CopernicaMarketingSoftware/AMQP-CPP?tab=readme-ov-file#publisher-confirms