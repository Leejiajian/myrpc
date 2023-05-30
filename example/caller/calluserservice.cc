#include<iostream>
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "user.pb.h"

int main(int argc , char** argv) {
    // 整个程序启动过后，使用mprpc框架来使用rpc服务调用
    MprpcApplication::Init(argc, argv);
    // 演示调用远程发布的rpc方法login
    lijiajian::UserServiceRpc_Stub stub(new MprpcChannel());
    lijiajian::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    lijiajian::LoginResponse response;

    MprpcController controller;
    // 发起rpc调用， 同步rpc调用过程   MprpcChannel::callmethod
    stub.Login(&controller, &request, &response, nullptr);   // 集中到RpcChannel-> RpcChannel::callMethod
    
    // 一次rpc调用完成，读取调用的结果
    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        std::cout << "rpc login response: " << response.result().errmsg() << std::endl;
    }
    // 演示调用远程发布的rpc方法Register
    lijiajian::RegisterRequest req;
    req.set_id(2000);
    req.set_name("mprpc");
    req.set_pwd("666666");
    lijiajian::RegisterResponse rsp;
    // 以同步方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr, &req, &rsp, nullptr);

        // 一次rpc调用完成，读取调用的结果
    if(rsp.result().errcode() == 0) {
        std::cout << "rpc register response:" << rsp.success() << std::endl;
    }
    else {
        std::cout << "rpc register response: " << rsp.result().errmsg() << std::endl;
    }

    //


    return 0;
}