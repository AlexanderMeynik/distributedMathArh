#include <amqpcpp.h>
#include <amqpcpp/linux_tcp.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>


#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>

#include <fmt/format.h>
#include <json/json.h>


std::string cString(std::string_view login,std::string_view password,std::string_view host,std::string_view port)
{
    return fmt::format("amqp://{}:{}@{}/vhost",login,password,host/*,port*/);
}
int main()
{//:syspassword http://localhost:15672
    // create an instance of your own tcp handler
    // access to the boost asio handler
    // note: we suggest use of 2 threads - normally one is fin (we are simply demonstrating thread safety).
    boost::asio::io_service service(4);

    // handler for libev
    AMQP::LibBoostAsioHandler handler(service);

    // make a connection
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://sysadmin:syspassword@localhost/"));

// and create a channel
    AMQP::TcpChannel channel(&connection);
    auto startCb = [](const std::string &consumertag) {

        std::cout << "consume operation started" << std::endl;
    };

// callback function that is called when the consume operation failed
    auto errorCb = [](const char *message) {

        std::cout << "consume operation failed" << std::endl;
    };

// callback operation when a message was received
    auto messageCb = [&channel](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {

        std::cout << fmt::format("message received:\"{}\"\n",std::string_view {message.body()});

        // acknowledge the message
        channel.ack(deliveryTag);
    };

// callback that is called when the consumer is cancelled by RabbitMQ (this only happens in
// rare situations, for example when someone removes the queue that you are consuming from)
    auto cancelledCb = [](const std::string &consumertag) {

        std::cout << "consume operation cancelled by the RabbitMQ server" << std::endl;
    };

// start consuming from the queue, and install the callbacks


    channel.consume("my-queue")
            .onReceived(messageCb)
            .onSuccess(startCb)
            .onCancelled(cancelledCb)
            .onError(errorCb);

    return service.run();

}