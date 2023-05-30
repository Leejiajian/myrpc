#include<iostream>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "friend.pb.h"
int main(int argc , char** argv) {
    // 整个程序启动过后，使用mprpc框架来使用rpc服务调用
    MprpcApplication::Init(argc, argv);
    // 演示调用远程发布的rpc方法login
    lijiajian::FriendServiceRpc_Stub stub(new MprpcChannel());
    lijiajian::GetFriendsListRequest request;
    request.set_userid(1000);

    lijiajian::GetFriendsListResponse response;
    // 发起rpc调用， 同步rpc调用过程   MprpcChannel::callmethod
    stub.GetFriendsList(nullptr, &request, &response, nullptr);   // 剧中到RpcChannel-> RpcChannel::callMethod
    
    // 一次rpc调用完成，读取调用的结果
    if(response.result().errcode() == 0) {
        std::cout << "rpc GetFriendsList response:" <<  std::endl;
        int size = response.friends_size();
        for(int i = 0; i < size; ++i) {
            std::cout << "index: " << i+1 << "name: " << response.friends(i) << std::endl;
        }
    }
    else {
        std::cout << "rpc GetFriendsList response: " << response.result().errmsg() << std::endl;
    }
    return 0;
}