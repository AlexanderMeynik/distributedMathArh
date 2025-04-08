#pragma once

#include <drogon/HttpController.h>
#include "computationalLib/math_core/TestRunner.h"
#include "common/sharedDeclarations.h"
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>

using namespace drogon;
using shared::FloatType;

namespace rest {
    namespace v1 {

        /// nodeStatus Enum
        enum class nodeStatus {
            /// Node is connected to cluster and is ready to receive it's tasks
            Active,
            /// Node is present in cluster but is not ready to recieve tasks
            Inactive,
            /// An error occurred moving node to a failed state
            Failed
        };

        //todo std::array
        /// Look-up table to cast nodeStatus to string
        const std::unordered_map<const nodeStatus, std::string> nodeStatusToStr
                {
                        {nodeStatus::Active,   "Active"},
                        {nodeStatus::Inactive, "Inactive"},
                        {nodeStatus::Failed,   "Failed"},
                };

        /**
         * @brief Computational node class
         */
        class computationalNode {
        public:
            HttpClientPtr httpClient;

            std::string getPath() {
                return httpClient->getHost() + ":" + std::to_string(httpClient->getPort());
            }

            std::valarray<FloatType> power;//todo spline function
            nodeStatus st;
        };

        /**
         * @brief Drogon service for main node
         */
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
