
#include "amqpCommon.h"

#include <fmt/format.h>
#include <thread>
//todo inherit from boost asio
using namespace amqpCommon;
int main()
{
    // create an instance of your own tcp handler
    // access to the boost asio handler
    // note: we suggest use of 2 threads - normally one is fin (we are simply demonstrating thread safety).
    boost::asio::io_service service(1);
    boost::asio::deadline_timer tm(service, boost::posix_time::seconds(1));




    // handler for libev
    AMQP::LibBoostAsioHandler handler(service);

    // make a connection
    AMQP::TcpConnection connection(&handler, AMQP::Address(adress));

    AMQP::TcpChannel channel(&connection);
    channel.onError([](const char* message)
                    {
                        std::cout<<"Channel error "<<message<<'\n';
                    });
    std::thread tr{[&tm,&connection,&channel,&service](){tm.wait();
        /*deleteQueue(channel,queue);
        deleteExchange(channel,exchange);*/
        channel.close();
        connection.close();//todo results in terminate(all tasks are handled)
        //service.stop();//abdruptly stops service
        }
    };//simple timer that will close everything
// and create a channel



    auto messageCb = [&channel,&tm](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {


        Json::Value val;
        Json::Reader read;
        read.parse(message.body(),val);
        std::cout << fmt::format("message received:\"size = {}, content = {}\"\n",message.bodySize(),val.toStyledString());

        // acknowledge the message
        channel.ack(deliveryTag);
        tm.expires_from_now(boost::posix_time::seconds(1));
    };


    channel.consume(queue)
            .onReceived(messageCb)
            .onSuccess(startCb)
            .onCancelled(cancelledCb)
            .onError(errorCb);



    service.run();

    return 0;

}