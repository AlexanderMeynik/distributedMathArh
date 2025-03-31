

/**
 *  Dependencies
 */
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/deadline_timer.hpp>


#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>
#include <json/json.h>
#include <iomanip>
//todo run without boost as event loop
//create and remove exchanges an queues
//retrieving queues and their names
//conenction create/remove
/**
 *  Main program
 *  @return int
 */
int main()
{

    // access to the boost asio handler
    // note: we suggest use of 2 threads - normally one is fin (we are simply demonstrating thread safety).
    boost::asio::io_service service(4);

    // handler for libev
    AMQP::LibBoostAsioHandler handler(service);

    // make a connection
    AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://sysadmin:syspassword@localhost/"));

    // we need a channel too
    AMQP::TcpChannel channel(&connection);
    channel.declareExchange("e");
    // create a temporary queue
    channel.declareQueue("my-queue").onSuccess([&connection](const std::string &name, uint32_t messagecount, uint32_t consumercount) {

        // report the name of the temporary queue
        std::cout << "declared queue " << name << std::endl;

        /*// now we can close the connection
        connection.close();*/
    });
    channel.bindQueue("e","my-queue","r");
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
        channel.publish("e","r",message.toStyledString());
    }




    return service.run();
}