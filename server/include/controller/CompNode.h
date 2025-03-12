#pragma once

#include <drogon/HttpController.h>

//https://github.com/drogonframework/drogon/wiki/ENG-04-2-Controller-HttpController
using namespace drogon;

namespace rest {
    namespace v1 {
        class CompNode : public drogon::HttpController<CompNode> {
            std::vector<std::string> Users;
        public:
            using cont=CompNode;
            METHOD_LIST_BEGIN
                ADD_METHOD_TO(cont::getInfo, "/user/{id}", Get);                  //path is /user/{arg1}
                ADD_METHOD_TO(cont::getDetailInfo, "/user/{id}/detailinfo", Get);  //path is /user/{arg1}/detailinfo
                ADD_METHOD_TO(cont::userList, "/list", Get);  //path is /list
                ADD_METHOD_TO(cont::newUser, "useradd/{name}", Post);                 //path is /useradd/{arg1}
            METHOD_LIST_END
            void getInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int userId) const;
            void getDetailInfo(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int userId) const;
            void newUser(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string &&userName);
            void userList(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
        };
    }
}
