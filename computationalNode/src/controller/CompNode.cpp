#include "controller/CompNode.h"
#include "common/Printers.h"

using namespace rest::v1;

void CompNode::getStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {


    Json::Value res;
    res["status"] = handler->con ? "running" : "not running";
    res["cc"] = *(handler->cc);

    res["bench"] = printUtils::continuousToJson(benchRes);

    callback(HttpResponse::newHttpJsonResponse(res));

}

void CompNode::connectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                              const std::string &ip, const std::string &name) {

    Json::Value res;

    res["input"] = ip;
    res["name"] = name;

    if (handler->con) {
        auto r = HttpResponse::newHttpJsonResponse(res);//toso cur service?
        r->setStatusCode(HttpStatusCode::k208AlreadyReported);
        callback(r);
        return;
    }
    handler->connect(ip, name);
    res["bench"] = printUtils::continuousToJson(benchRes);
    auto r = HttpResponse::newHttpJsonResponse(res);
    callback(r);

}

void CompNode::disconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value res;
    res["request"] = "disconnect";

    if (!handler->con) {
        auto r = HttpResponse::newHttpJsonResponse(res);
        r->setStatusCode(HttpStatusCode::k409Conflict);
        callback(r);
        return;
    }
    res["cc"] = *(handler->cc);
    auto r = HttpResponse::newHttpJsonResponse(res);
    handler->disconnect();


    callback(r);

}
