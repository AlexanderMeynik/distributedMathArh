#include "controller/CompNode.h"

using namespace rest::v1;

void
CompNode::getInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                  int userId) const {
    if(Users.size()<=userId)
    {
        callback(HttpResponse::newNotFoundResponse(req));
        return;
    }

    Json::Value ret;
    ret["ret"]["userId"] = userId;
    ret["ret"]["userName"] = Users[userId];

    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void CompNode::getDetailInfo(const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback,
                             int userId) const {
    if(Users.size()<=userId)
    {
        callback(HttpResponse::newNotFoundResponse(req));
        return;
    }

    Json::Value ret;
    //ret["ret"]=Json::Value();
    ret["ret"]["userId"] = userId;
    ret["ret"]["userName"] = Users[userId];
    time_t _tm =time(NULL );

    struct tm * curtime = localtime ( &_tm );
    ret["timestamp"]=asctime(curtime);
    auto resp = HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void
CompNode::newUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback,
                  std::string &&userName) {
    Users.push_back(userName);
    auto a=HttpResponse::newHttpResponse(HttpStatusCode::k200OK,drogon::ContentType::CT_APPLICATION_JSON);
    callback(a);
}

void CompNode::userList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    Json::Value ret;
    //ret["ret"]=Json::Value();
    //ret["ret"]["size"]=Json::Value();
    //ret["ret"]["data"]=Json::Value(Json::ValueType::arrayValue);
    if(Users.size())
    {
        ret["ret"]["size"]=Users.size();
    }

    for (int i = 0; i < Users.size(); ++i) {
        ret["ret"]["data"][i]=Users[i];
    }


    time_t _tm =time(NULL );

    struct tm * curtime = localtime ( &_tm );
    ret["timestamp"]=asctime(curtime);
    callback(HttpResponse::newHttpJsonResponse(ret));
}
