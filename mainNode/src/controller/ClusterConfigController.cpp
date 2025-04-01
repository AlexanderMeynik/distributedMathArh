#include "controller/ClusterConfigController.h"
#include "common/Parsers.h"
#include "common/Printers.h"

using namespace rest::v1;

void
ClusterConfigController::getStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    /*auto r=HttpResponse::newHttpResponse();*/

    Json::Value root;

    int i = 0;
    root["size"] = clients.size();
    for (auto &[str, node]: clients) {
        //root["data"][i]=Json::Value();
        root["data"][i]["host"] = str;
        root["data"][i]["status"] = mapss.at(node.st);
        root["data"][i]["benchRes"]= printUtils::continuousToJson(node.power);
        i++;
    }
    //todo call all of nodes;

    callback(HttpResponse::newHttpJsonResponse(root));

}

void ClusterConfigController::connectHandler(const HttpRequestPtr &req,
                                             std::function<void(const HttpResponsePtr &)> &&callback,
                                             const std::string &hostPort, const std::string &qip,
                                             const std::string &name) {

    Json::Value res;

    if (!clients.count(hostPort)) {
        computationalNode cn;
        //todo ad https as option
        cn.httpClient = HttpClient::newHttpClient("http://" + hostPort);
        cn.st = NodeStatus::inactive;
        clients[hostPort] = std::move(cn);
    }

    auto req1 = HttpRequest::newHttpRequest();

    req1->setPath("/v1/connect");
    req1->setParameter("ip", qip);
    req1->setParameter("name", name);
    req1->setMethod(Post);


    auto [code, resp] = clients[hostPort].httpClient->sendRequest(req1);
    auto jsonPtr = resp->jsonObject();


    if (!resp) {
        res["ip"] = hostPort;
        res["qip"] = qip;
        res["qname"] = name;
        res["message"] = "Unable to connect to node";
        auto r = HttpResponse::newHttpJsonResponse(res);
        callback(r);
        return;//todo maybe some guard liek class to handle this
    }
    if (resp->getStatusCode() != HttpStatusCode::k200OK) {

        res["ip"] = hostPort;
        res["qip"] = qip;
        res["qname"] = name;
        res["code"] = resp->getStatusCode();
        auto r = HttpResponse::newHttpJsonResponse(res);
        callback(r);
        return;
    }

    clients[hostPort].power = printUtils::jsonToContinuous<std::valarray<double>>((*jsonPtr)["bench"]);
    res["ip"] = hostPort;
    res["qip"] = qip;
    res["qname"] = name;
    clients[hostPort].st = NodeStatus::active;
    res["benchRes"] = printUtils::continuousToJson(clients[hostPort].power);

    auto r = HttpResponse::newHttpJsonResponse(res);


    callback(r);

}

void ClusterConfigController::disconnectHandler(const HttpRequestPtr &req,
                                                std::function<void(const HttpResponsePtr &)> &&callback,
                                                const std::string &hostPort) {
    Json::Value res;

    auto req1 = HttpRequest::newHttpRequest();

    req1->setPath("/v1/disconnect");
    req1->setMethod(Post);

    auto ct = HttpClient::newHttpClient(hostPort);
    auto [code, resp] = clients[hostPort].httpClient->sendRequest(req1);

    if (resp->getStatusCode() != HttpStatusCode::k200OK) {
        if (!resp->body().empty())//todo use this for conenct if body is empty
        {
            res = *resp->getJsonObject();
        }

        res["ip"] = hostPort;
        res["code"] = resp->getStatusCode();

        auto r = HttpResponse::newHttpJsonResponse(res);
        callback(r);
        return;
    }

    clients[hostPort].st = NodeStatus::inactive;
    //todo what to do with http client;

    res = *resp->getJsonObject();
    res["ip"] = hostPort;
    res["benchRes"] = printUtils::continuousToJson(clients[hostPort].power);


    auto r = HttpResponse::newHttpJsonResponse(res);


    callback(r);

}
