//#include <cpprest/http_client.h>
//#include <cpprest/filestream.h>
//#include <iostream>
//
//using namespace utility;
//using namespace web;
//using namespace web::http;
//using namespace web::http::client;
//
//void make_get_request(const uri &uri) {
//    http_client client(uri);
//
//    client.request(methods::GET).then([](http_response response) {
//        if (response.status_code() == status_codes::OK) {
//            return response.extract_string();
//        }
//        return pplx::task_from_result(utility::string_t());
//    }).then([](pplx::task<utility::string_t> previousTask) {
//        try {
//            std::wcout << previousTask.get().data() << std::endl;
//        } catch (const http_exception &e) {
//            std::wcout << e.what() << std::endl;
//        }
//    }).wait();
//}
//
//void make_post_request(const uri &uri, const json::value &body) {
//    http_client client(uri);
//
//    http_request request(methods::POST);
//    request.headers().set_content_type(U("application/json"));
//    request.set_body(body);
//
//    client.request(request).then([](http_response response) {
//        if (response.status_code() == status_codes::OK) {
//            return response.extract_string();
//        }
//        return pplx::task_from_result(utility::string_t());
//    }).then([](pplx::task<utility::string_t> previousTask) {
//        try {
//            std::wcout << previousTask.get().data() << std::endl;
//        } catch (const http_exception &e) {
//            std::wcout << e.what() << std::endl;
//        }
//    }).wait();
//}
//
//int main() {
//    //make_get_request(U("http://localhost:18080/"));
//    //make_get_request(U("http://localhost:18080/json"));
//    //make_get_request(U("http://localhost:18080/add/3/4"));
//    make_get_request(U("http://localhost:18080/calculate/2/4/1.0e-6"));//todo post лучше
//
//    // Create JSON body for POST request
//    json::value postData;
//    postData[U("name")] = json::value::string(U("John Doe"));
//    postData[U("age")] = json::value::number(30);
//
//    make_post_request(U("http://localhost:18080/post"), postData);
//
//    return 0;
//}
/*
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

int main(int argc, char* argv[])
{
    auto fileStream = std::make_shared<ostream>();

    // Open stream to output file.
    pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
                                                                                 {
                                                                                     *fileStream = outFile;

                                                                                     // Create http_client to send the request.
                                                                                     http_client client(U("http://www.bing.com/"));

                                                                                     // Build request URI and start the request.
                                                                                     uri_builder builder(U("/search"));
                                                                                     builder.append_query(U("q"), U("cpprestsdk github"));
                                                                                     return client.request(methods::GET, builder.to_string());
                                                                                 })

                    // Handle response headers arriving.
            .then([=](http_response response)
                  {
                      printf("Received response status code:%u\n", response.status_code());

                      // Write response body into the file.
                      return response.body().read_to_end(fileStream->streambuf());
                  })

                    // Close the file stream.
            .then([=](size_t)
                  {
                      return fileStream->close();
                  });

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try
    {
        requestTask.wait();
    }
    catch (const std::exception &e)
    {
        printf("Error exception:%s\n", e.what());
    }

    return 0;
}*/


#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

int main() {
    // Create a file stream to write the received file into it.
    auto fileStream = std::make_shared<ostream>();

    // Open stream to output file.
    pplx::task<void> requestTask = fstream::open_ostream(U("users.json"))

            // Make a GET request.
            .then([=](ostream outFile) {
                *fileStream = outFile;
               // http://localhost:18080/calculate/2/4/1.0e-6
                // Create http_client to send the request.
                http_client client(U("http://localhost:18080"));

                http://localhost:18080/calculate/2/4/1.0e-6
                // Build request URI and start the request.
                return client.request(methods::GET, uri_builder(U("calculate")).append_path(U(std::to_string(2))).append_path(U(std::to_string(4)))
                .append_path(U(std::to_string(1.0e-6)))
                .to_string());
            })

                    // Get the response.
            .then([=](http_response response) {
                // Check the status code.
                if (response.status_code() != 200) {
                    throw std::runtime_error("Returned " + std::to_string(response.status_code()));
                }

                // Write the response body to file stream.
                response.body().read_to_end(fileStream->streambuf()).wait();

                // Close the file.
                return fileStream->close();
            });

    // Wait for the concurrent tasks to finish.
    try {
        while (!requestTask.is_done()) { std::cout << "."; }
    } catch (const std::exception &e) {
        printf("Error exception:%s\n", e.what());
    }

    return 0;
}