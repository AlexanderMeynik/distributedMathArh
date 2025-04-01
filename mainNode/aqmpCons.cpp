
#include "amqpCommon.h"

#include <fmt/format.h>

using namespace amqpCommon;
int main()
{
    // create an instance of your own tcp handler
    // access to the boost asio handler
    // note: we suggest use of 2 threads - normally one is fin (we are simply demonstrating thread safety).
    boost::asio::io_service service(1);

    // handler for libev
    AMQP::LibBoostAsioHandler handler(service);

    // make a connection
    AMQP::TcpConnection connection(&handler, AMQP::Address(adress));

// and create a channel
    AMQP::TcpChannel channel(&connection);

    auto messageCb = [&channel](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {

        //Json::Value vla(message.body());

        std::cout << fmt::format("message received:\"{}\"\n",message.body());

        // acknowledge the message
        channel.ack(deliveryTag);
    };


    channel.consume(queue)
            .onReceived(messageCb)
            .onSuccess(startCb)
            .onCancelled(cancelledCb)
            .onError(errorCb);


    //channel.removeExchange(exchange);

    return service.run();

}