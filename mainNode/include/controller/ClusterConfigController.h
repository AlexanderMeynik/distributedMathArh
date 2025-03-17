#pragma once

#include <drogon/HttpController.h>
#include "computationalLib/math_core/TestRunner.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

using namespace drogon;
//todo move away


namespace rest {
    namespace v1 {

        enum class NodeStatus {
            active,
            inactive,
            failed

        };

        const std::unordered_map<const NodeStatus, std::string> mapss


                {
                        {NodeStatus::active,   "active"},
                        {NodeStatus::inactive, "inactive"},
                        {NodeStatus::failed,   "failed"},
                };

        class computationalNode {
        public:
            HttpClientPtr httpClient;

            std::string getPath() {
                return httpClient->getHost() + ":" + std::to_string(httpClient->getPort());
            }

            std::valarray<double> power;//todo spline function
            NodeStatus st;


        };

        class ClusterConfigController : public drogon::HttpController<ClusterConfigController> {
            std::unordered_map<std::string, computationalNode> clients;
        public:
            ClusterConfigController() {
                //client=HttpClient::newHttpClient("http://localhost:8081");
                //client->get

            }//todo /rebalance
            //todo ping(measures latencies)?
            using cont = ClusterConfigController;

            METHOD_LIST_BEGIN

                ADD_METHOD_TO(cont::getStatus, "v1/status", Get);
                ADD_METHOD_TO(cont::connectHandler, "v1/connect?ip={ip}&qip={qip}&name={queue}", Post);
                ADD_METHOD_TO(cont::disconnectHandler, "v1/disconnect?ip={ip}", Post);
            METHOD_LIST_END

            void getStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

            void connectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                                const std::string &hostPort, const std::string &qip, const std::string &name);

            void disconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                                   const std::string &hostPort);
        };
    }
}
