
#include "amqpCommon.h"
#include <thread>



//todo erro handling(can i throw exception from callbacks)_ how can i notify the main programm





static volatile std::sig_atomic_t signalReceived = 0;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        signalReceived = 1;
    }
}

int main(int argc,char * argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <cString> <queue_name>\n";
        return 1;
    }
    auto cString=argv[1];
    auto qname=argv[2];

    std::signal(SIGINT, signalHandler);
    using namespace amqpCommon;

    amqpConsumerService listener(cString, qname);
    listener.connect();


    while (!signalReceived) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    listener.disconnect();
    return 0;
}
