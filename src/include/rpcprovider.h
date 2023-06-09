#pragma once
#include "google/protobuf/service.h"
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
// 框架提供的专门发布rpc服务的网络对象类
class RpcProvider {
    public:
        //框架提供给外部使用，可以发布rpc方法的函数接口
        void NotifyService(google::protobuf::Service *service) ;
        // 启动rpc服务节点开始提供服务
        void Run();

    private:
        // 组合TcpServer
        // std::unique_ptr<muduo::net::TcpServer> m_tcpserverPtr;
        // 组合EventLoop
        muduo::net::EventLoop m_eventLoop;
        // service 服务类型信息
        struct ServiceInfo {
            google::protobuf::Service *m_service;   // 保存服务对象
            std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; //保存服务

        };
        // 存储注册成功的服务对象和其服务方法的所有信息
        std::unordered_map<std::string, ServiceInfo> m_serviceMap;
        // socket连接回调
        void OnConnection(const muduo::net::TcpConnectionPtr&);
        
        void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
        // Closure回调操作，用于序列化rpc的响应和网络发送
        void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};