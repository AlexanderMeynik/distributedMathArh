#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <iostream>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

void make_get_request(const uri& uri) {
    http_client client(uri);

    client.request(methods::GET).then([](http_response response) {
        if (response.status_code() == status_codes::OK) {
            return response.extract_string();
        }
        return pplx::task_from_result(utility::string_t());
    }).then([](pplx::task<utility::string_t> previousTask) {
        try {
            std::wcout << previousTask.get().data() << std::endl;
        } catch (const http_exception& e) {
            std::wcout << e.what() << std::endl;
        }
    }).wait();
}

void make_post_request(const uri& uri, const json::value& body) {
    http_client client(uri);

    http_request request(methods::POST);
    request.headers().set_content_type(U("application/json"));
    request.set_body(body);

    client.request(request).then([](http_response response) {
        if (response.status_code() == status_codes::OK) {
            return response.extract_string();
        }
        return pplx::task_from_result(utility::string_t());
    }).then([](pplx::task<utility::string_t> previousTask) {
        try {
            std::wcout << previousTask.get().data() << std::endl;
        } catch (const http_exception& e) {
            std::wcout << e.what() << std::endl;
        }
    }).wait();
}

int main() {
    //make_get_request(U("http://localhost:18080/"));
    //make_get_request(U("http://localhost:18080/json"));
    //make_get_request(U("http://localhost:18080/add/3/4"));
    make_get_request(U("http://localhost:18080/calculate/2/4/1.0e-6"));

    // Create JSON body for POST request
    json::value postData;
    postData[U("name")] = json::value::string(U("John Doe"));
    postData[U("age")] = json::value::number(30);

    make_post_request(U("http://localhost:18080/post"), postData);

    return 0;
}
