#pragma once

#include <drogon/HttpController.h>
#include "computationalLib/math_core/TestRunner.h"


using namespace drogon;

namespace rest {
    namespace v1 {
        class AtmqHandler
        {
        public:
            AtmqHandler()
            {
                cc=std::make_shared<int>(0);
                con=false;
            }
            bool checkConnection(std::string &ip,std::string&queue)
            {
                //todo check where logs are written
                con= true;
                LOG_INFO<<"checkConnection\t"<<ip<<'\t'<<queue<<'\n';
                return con;
            }
            bool connect(std::string &ip,std::string&queue)
            {
                con= true;
                LOG_INFO<<"connect\t"<<ip<<'\t'<<queue<<'\n';
                queues[0]=queue+"1";
                queues[1]=queue+"2";

                eventLoop=std::jthread([this](std::stop_token stoken) {
                    //todo listen to queues(from 1 to second)
                    while (con)
                    {
                        sleep(1);
                        std::stringstream ss;
                        ss<<std::this_thread::get_id();
                        LOG_INFO<<ss.str()<<'\t'<<*cc<<'\n';
                        *cc=*cc+1;
                    }
                });

                return true;
            }
            void disconenct()
            {
                
                con= false;
                eventLoop.join();
                reset();
                LOG_INFO<<"disconnect\n";
            }
            int getC()
            {
                return *cc;
            }
            void reset()
            {
                *cc=0;
            }

            std::atomic<bool> con;
            std::array<std::string ,2>queues;//todo chnage to queueq handlers
            std::jthread eventLoop;
            std::shared_ptr<int>cc;
            //maybe store dispatch here.
            //todo atmqclient
        };
        class ClusterConfigController : public drogon::HttpController<ClusterConfigController> {
            std::unordered_map<std::string,std::thread> thrreads;
            std::shared_ptr<AtmqHandler> handler;
        public:
            ClusterConfigController()
            {
                handler=std::make_shared<AtmqHandler>();
            }
            using cont=ClusterConfigController;
            METHOD_LIST_BEGIN
                ADD_METHOD_TO(cont::getStatus, "v2/status", Get);
                ADD_METHOD_TO(cont::connectHandler, "v2/connect?ip={ip}&name={queue}", Post);
                ADD_METHOD_TO(cont::disconnectHandler, "v2/disconnect", Post);
            METHOD_LIST_END
            void getStatus(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const;
            void connectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string &&ip, std::string&& name) const;
            void disconnectHandler(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
        };
    }
}
