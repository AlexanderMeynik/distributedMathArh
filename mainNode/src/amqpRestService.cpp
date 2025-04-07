#include "amqpRestService.h"
#include <sstream>
#include "common/errorHandling.h"

namespace amqpCommon {

    RabbitMQRestService::RabbitMQRestService(const std::string& baseUrl,
                                             authHandler*authHandler)
            :baseUrl(baseUrl),
            m_authPtr(authHandler){
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    RabbitMQRestService::~RabbitMQRestService() {
        curl_global_cleanup();
    }


    std::string RabbitMQRestService::performRequest(const std::string& path,
                                                    const std::string& method,
                                                    const std::string& data) {
        return performCurlRequest(path,method,baseUrl,m_authPtr,data);
    }

    Json::Value RabbitMQRestService::parseJson(const std::string& jsonStr) {
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;
        std::istringstream iss(jsonStr);
        if (!Json::parseFromStream(builder, iss, &root, &errs)) {
            throw std::runtime_error("JSON parse error: " + errs);
        }
        return root;
    }

    bool RabbitMQRestService::createQueue(const std::string& vhost,
                                          const std::string& queueName,
                                          const Json::Value& arguments) {
        std::string path = fmt::format("/api/queues/{}/{}",vhost,queueName);
        Json::Value body;
        body["auto_delete"] = false;
        body["durable"] = true;
        body["arguments"] = arguments;
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "PUT", data);
        return true;
    }

    bool RabbitMQRestService::deleteQueue(const std::string& vhost,
                                          const std::string& queueName) {
        std::string path =fmt::format("/api/queues/{}/{}",vhost,queueName);
        performRequest(path, "DELETE");
        return true;
    }

    bool RabbitMQRestService::createExchange(const std::string &vhost, const exchange&exchange,
                                             const Json::Value &arguments) {
        std::string path =fmt::format("/api/exchanges/{}/{}",vhost,exchange.name);
        Json::Value body=exchange.dat.toJson();
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "PUT", data);
        return true;
    }

    bool RabbitMQRestService::deleteExchange(const std::string &vhost, const std::string &exchangeName) {
        std::string path = fmt::format("/api/exchanges/{}/{}",vhost,exchangeName);
        performRequest(path, "DELETE");
        return true;
    }


    Json::Value RabbitMQRestService::getQueueStats(const std::string& vhost,
                                                   const std::string& queueName) {


        std::string path = fmt::format("/api/queues/{}/{}",vhost,queueName);
        std::string response = performRequest(path, "GET");
        return parseJson(response);
    }

    std::vector<std::string> RabbitMQRestService::listQueues(const std::string& vhost) {
        std::string path = "/api/queues/" + vhost;
        std::string response = performRequest(path, "GET");
        Json::Value j = parseJson(response);
        std::vector<std::string> queues;
        for (const auto& item : j) {
            queues.push_back(item["name"].asString());
        }
        return queues;
    }

    bool RabbitMQRestService::bindQueueToExchange(const std::string& vhost,
                                                  const std::string& queueName,
                                                  const std::string& exchangeName,
                                                  const std::string& routingKey) {
        std::string path = "/api/bindings/" + vhost + "/e/" + exchangeName + "/q/" + queueName;
        Json::Value body;
        body["routing_key"] = routingKey;
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "POST", data);
        return true;
    }

    bool RabbitMQRestService::unbindQueueFromExchange(const std::string& vhost,
                                                      const std::string& queueName,
                                                      const std::string& exchangeName,
                                                      const std::string& routingKey) {
        std::string path = "/api/bindings/" + vhost + "/e/" + exchangeName + "/q/" + queueName + "/" + routingKey;
        performRequest(path, "DELETE");
        return true;
    }

    bool RabbitMQRestService::createUser(const std::string& user,
                                         const std::string& pass) {
        std::string path = "/api/users/" + user;
        Json::Value body;
        body["password"] = pass;
        body["tags"] = "worker";
        std::string data = Json::writeString(Json::StreamWriterBuilder(), body);
        performRequest(path, "PUT", data);
        return true;
    }

    bool RabbitMQRestService::deleteUser(const std::string& user) {
        std::string path = "/api/users/" + user;
        performRequest(path, "DELETE");
        return true;
    }

    std::vector<rabbitMQUser> RabbitMQRestService::listUsers(const std::string &vhost) {
        std::string path = "/api/users/";

        std::vector<rabbitMQUser> out;

        auto res=parseJson(performRequest(path, "GET"));
        if(res.isArray())
        {
            out.reserve(res.size());
            for (auto & re : res) {
                out.emplace_back(re);
            }
        }
        return out;
    }

    Json::Value RabbitMQRestService::whoami() {

        std::string path = "/api/whoami";

        std::string response = performRequest(path, "GET");

        return parseJson(response);
    }

    std::vector<queueBinding> RabbitMQRestService::getQueueBindings(const std::string &vhost,const std::string &queue) {
        std::string path=fmt::format("/api/queues/{}/{}/bindings",vhost,queue);

        auto res= parseJson(performRequest(path,"GET"));
        std::vector<queueBinding> out;
        if(res.isArray())
        {
            for (auto & re : res) {
                out.emplace_back(re);
            }
        }

        return out;
    }

    std::vector<exchange> RabbitMQRestService::getExchanges(const std::string &vhost) {
        std::string path=fmt::format("/api/exchanges/{}",vhost);

        auto res= parseJson(performRequest(path,"GET"));
        std::vector<exchange> out;
        if(res.isArray())
        {
            for (auto & re : res) {
                out.emplace_back(re);
            }
        }

        return out;
    }




}