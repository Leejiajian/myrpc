#include<iostream>
#include<string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <vector>
//using namespace lijiajian;

class FriendService : public lijiajian::FriendServiceRpc{
    public:
        std::vector<std::string> GetFriendsList(uint32_t userid) {
            std::cout << "don GetFriendList service" << std::endl;
            std::vector<std::string> vec;
            vec.push_back("abc");
            vec.push_back("lijiajian");
            return vec;
        }
        void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::lijiajian::GetFriendsListRequest* request,
                       ::lijiajian::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done) {
            uint32_t userid = request->userid();
            std::vector<std::string> friendsList = GetFriendsList(userid);
            response->mutable_result()->set_errcode(0);
            response->mutable_result()->set_errmsg("");
            for(std::string &name : friendsList) {
                std::string *p = response->add_friends();
                *p = name;
            }
            done->Run();
        }
};
int main(int argc, char** argv) {
        // 调用框架初始化
    MprpcApplication::Init(argc, argv);
    // 把服务对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());
    // 启动rpc服务
    provider.Run();
    return 0;
}