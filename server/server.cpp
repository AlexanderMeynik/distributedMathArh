#define _TURN_OFF_PLATFORM_STRING
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>
#include <map>
#include "../application/math_core/TestRunner.h"
//#include "../application/TestRunner.h"
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

void handle_get(http_request request) {
    ucout << request.to_string() << std::endl;

    auto path = uri::split_path(uri::decode(request.relative_uri().path()));

    if (path.empty()) {
        request.reply(status_codes::OK, _XPLATSTR("Hello, world!"));
    } else if (path[0] == _XPLATSTR("json")) {
        json::value jsonResponse;
        jsonResponse[_XPLATSTR("message")] = json::value::string(_XPLATSTR("Hello, world!"));
        jsonResponse[_XPLATSTR("number")] = json::value::number(123);
        request.reply(status_codes::OK, jsonResponse);
    } else if (path[0] == _XPLATSTR("add") && path.size() == 3) {
        try {
            int a = std::stoi(path[1]);
            int b = std::stoi(path[2]);
            json::value jsonResponse;
            jsonResponse[_XPLATSTR("result")] = json::value::number(a + b);
            request.reply(status_codes::OK, jsonResponse);
        } catch (const std::exception& e) {
            request.reply(status_codes::BadRequest, _XPLATSTR("Invalid parameters"));
        }

    }
    else if (path[0] == _XPLATSTR("calculate") && path.size() == 4) {
        try {
            int N = std::stoi(path[1]);
            int Ns = std::stoi(path[2]);
            double arange = std::stod(path[3]);
            TestRunner ts(N,Ns,arange);
            //auto gen=GausGenerator<double>(0,arange,N);
            auto gen2=TriangGenerator<double>(0,0,arange/4.0,arange,false);
            ts.generateCoords(gen2);//todo потестить
            ts.solve();


            std::stringstream ss;
            for (int i = 0; i < Ns; ++i) {
                printSolutionFormat1(ss,ts.getSolRef()[i]);
            }

            json::value jsonResponse;
            jsonResponse[_XPLATSTR("result")] = json::value::string(ss.str());
            request.reply(status_codes::OK, jsonResponse);

        } catch (const std::exception &e) {
            request.reply(status_codes::BadRequest, _XPLATSTR("Invalid parameters"));
        }
    }
    else {
        request.reply(status_codes::NotFound, _XPLATSTR("Not Found"));
    }
}

void handle_post(http_request request) {//todo создать сервис для обработки post запросов(что-то типа хэш таблицы для поиска функции по методу)
    ucout << request.to_string() << std::endl;

    request.extract_json().then([request](pplx::task<json::value> task) {
        try {
            auto json = task.get();
            ucout << _XPLATSTR("Received POST request with body: ") << json.serialize() << std::endl;

            // Echo back the received JSON
            request.reply(status_codes::OK, json);
        } catch (const http_exception& e) {
            request.reply(status_codes::BadRequest, _XPLATSTR("Invalid JSON"));
        }
    }).wait();
}

int main() {
    uri_builder uri(_XPLATSTR("http://localhost:18080"));
    auto addr = uri.to_uri().to_string();
    http_listener listener(addr);

    listener.support(methods::GET, handle_get);
    listener.support(methods::POST, handle_post);

    try {
        listener
                .open()
                .then([&listener]() { ucout << _XPLATSTR("Starting to listen at: ") << listener.uri().to_string() << std::endl; })
                .wait();

        std::string line;
        std::getline(std::cin, line);
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}
