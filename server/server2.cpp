#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <mutex>
#include <thread>
#include <memory>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;


class AuthService {
public:
    static bool validate_token(const utility::string_t& token) { return token == U("mock_token"); }
    static bool is_sysadmin(const utility::string_t& token) { /* DB check */ return true; }
    static utility::string_t create_token() { return U("mock_token"); }
};


void require_auth(http_request request, std::function<void(http_request)> handler) {
    auto headers = request.headers();
    if (!headers.has(U("Authorization")) ||
        !AuthService::validate_token(headers[U("Authorization")])) {
        request.reply(status_codes::Unauthorized);
        return;
    }
    handler(request);
}


class ComputationalNode {
    std::mutex queue_mutex;
    std::unique_ptr<http_listener> queue_listener;
    std::thread listener_thread;
    bool running = false;
    std::vector<std::string> messages;

public:
    void setup_listener(const utility::string_t& queue_name) {
        http_listener_config config;
        uri_builder uri(U("http://localhost:8081"));
       // uri.append_path(U("queue")).append_path(queue_name);

        queue_listener = std::make_unique<http_listener>(uri.to_uri(), config);

        queue_listener->support(methods::GET, [&](http_request request) {

            auto path = uri::decode(request.request_uri().path());
            if(path.find(U("/list")) != 0)
            {
                request.reply(status_codes::NotFound);
            }

            web::json::value json_array = web::json::value::array();

            for (int i = 0; i < messages.size(); ++i) {
                json_array[i]=web::json::value::string(messages[i]);
            }


            json::value jsonResponse;
            web::json::value structure = web::json::value::object(true);


            structure[U("data")] = json_array;
            jsonResponse[U("retStruct")]=structure;


            time_t _tm =time(NULL );

            struct tm * curtime = localtime ( &_tm );
            //cout<<"The current date/time is:"<<asctime(curtime);

            jsonResponse[U("timestamp")]=web::json::value::string(asctime(curtime));
            request.reply(status_codes::OK, jsonResponse);

        });


        queue_listener->support(methods::POST, [&](http_request request) {

            json::value jsonResponse;
            auto path = uri::decode(request.request_uri().path());
            if(path.find(U("/queue")) != 0)
            {
                request.reply(status_codes::NotFound);
            }

            json::value json=request.extract_json().get();

            auto mess=json[U("message")].as_string();
            messages.push_back(mess);

            time_t _tm =time(NULL );

            struct tm * curtime = localtime ( &_tm );

            jsonResponse[U("timestamp")]=web::json::value::string(asctime(curtime));
            std::random_shuffle(mess.begin(), mess.end());
            jsonResponse[U("reply")]=web::json::value::string(mess);
            request.reply(status_codes::OK, jsonResponse);



        });

        running = true;
        listener_thread = std::thread([this]() {
            try {
                queue_listener->open().wait();
                /*while(running) std::this_thread::sleep_for(std::chrono::milliseconds(100));*/
            } catch (const std::exception& e) {
                std::cerr << "Error opening queue listener: " << e.what() << std::endl;
            }
        });
    }

    void handle_connect(const http_request& request, const utility::string_t& queue_name) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (!queue_listener) {
            setup_listener(queue_name);
            request.reply(status_codes::OK, U("Connected with queue"));
        } else {
            request.reply(status_codes::Conflict, U("Already connected"));
        }
    }

    void handle_disconnect(const http_request& request) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (queue_listener) {
            running = false;
            queue_listener->close().wait();
            listener_thread.join();
            queue_listener.reset();
            messages.clear();
            request.reply(status_codes::OK);

        } else {
            request.reply(status_codes::OK, U("Wasn't connected"));
        }
    }
};

int main() {
    // Main node listener
    http_listener main_listener(U("http://localhost:8080"));
    ComputationalNode comp_node;

    auto auth_wrapper = [](const std::function<void(http_request)>& handler) {
        return [handler](http_request request) {
            require_auth(request, handler);
        };
    };

    main_listener.support(methods::GET, [](http_request request) {
        auto path = uri::decode(request.request_uri().path());
        if (path.find(U("/logIn")) == 0) {
            auto segments = uri::split_path(path);
            if (segments.size() >= 3 && segments[0] == U("logIn")) {
                // Extract login and password from URL
                auto& login = segments[1];
                auto& password = segments[2];

                // Authentication logic here
                request.reply(status_codes::OK, U("mock_token"));
                return;
            }
        }
        request.reply(status_codes::NotFound);
    });



    main_listener.support(methods::POST, auth_wrapper([&comp_node](http_request request) {
        auto path = uri::decode(request.request_uri().path());
        if (path.find(U("/connect")) == 0) {
            auto segments = uri::split_path(path);
            if (segments.size() >= 2 && segments[0] == U("connect")) {
                auto& queue_name = segments[1];
                comp_node.handle_connect(request, queue_name);
                return;
            }
        }

        if (path.find(U("/disconnect")) == 0) {
            auto segments = uri::split_path(path);
            if (segments.size() >= 1 && segments[0] == U("disconnect")) {
                auto& queue_name = segments[1];
                comp_node.handle_disconnect(request);
                return;
            }
        }
        request.reply(status_codes::NotFound);

    }));





    try {
        main_listener.open().wait();
        std::cout << "Main node listening..." << std::endl;
        while (true);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}