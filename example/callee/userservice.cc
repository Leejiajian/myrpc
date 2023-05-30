#include<iostream>
#include<string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"
using namespace lijiajian;
/*
    UserService原来是本地服务，提供两个进程的本地方法，Login和GetFriendLists
*/  // rpc服务发布端
class UserService : public lijiajian::UserServiceRpc{
    bool Login(std::string &name, std::string pwd) {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }
    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }
    // 重写基类UserServiceRpc的虚函数   1. 从LoginRequest获取参数的值 2.执行本地服务Login, 并获取返回值
    // 3.用上边的返回值填写LoginResponse 4.一个回调，把LoginResponse 发送给rpc client
    void Login(::google::protobuf::RpcController* controller,
                       const ::lijiajian::LoginRequest* request,
                       ::lijiajian::LoginResponse* response,
                       ::google::protobuf::Closure* done) 
    {
        // 框架给业务上报了请求参数LoginRequest, 应用获取相应数据做本地服务
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool login_result = Login(name, pwd); //做本地业务
        // 把响应写入
        lijiajian::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);
        
        // 执行回调操作, 执行响应对象数据的序列化和网络发送(由框架来做)
        done->Run();
    }
    void Register(::google::protobuf::RpcController* controller,
                         const ::lijiajian::RegisterRequest* request,
                         ::lijiajian::RegisterResponse* response,
                         ::google::protobuf::Closure* done) {
        std::string name = request->name();
        std::string pwd = request->pwd();
        uint32_t id = request->id();
        bool register_result = Register(id, name, pwd); 
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(register_result);
        done->Run();
    }

};

int main(int argc, char **argv) { 
    LOG_INFO("first log message!");
    LOG_INFO("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
    // 调用框架初始化
    MprpcApplication::Init(argc, argv);
    // 把服务对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());
    // 启动rpc服务
    provider.Run();
    return 0;
}
