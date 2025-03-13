#include "controller/CompNode.h"

using namespace rest::v1;

void CompNode::getStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const {
    /*auto r=HttpResponse::newHttpResponse();*/
    Json::Value ret;
    ret["status"]=handler->con?"running":"not running";
    ret["cc"]=*(handler->cc);
    callback(HttpResponse::newHttpJsonResponse(ret));

}

void CompNode::connectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                              std::string&& ip, std::string&& name) const {

    Json::Value res;

    res["input"]=ip;
    res["name"]=name;
    auto r=HttpResponse::newHttpJsonResponse(res);
    if(handler->con)
    {
        r->setStatusCode(HttpStatusCode::k208AlreadyReported);
        callback(r);
        return;
    }
    handler->connect(ip,name);

    callback(r);

}

void CompNode::disconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value res;
    res["request"]="disconnect";

    if(!handler->con)
    {
        auto r=HttpResponse::newHttpJsonResponse(res);
        r->setStatusCode(HttpStatusCode::k409Conflict);
        callback(r);
        return;
    }
    res["cc"]=*(handler->cc);
    auto r=HttpResponse::newHttpJsonResponse(res);
    handler->disconenct();


    callback(r);

}
