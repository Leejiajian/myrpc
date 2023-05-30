#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"
#define THREADED
//框架提供给外部使用，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service)  {
    ServiceInfo service_info;
    // 获取服务的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = pserviceDesc->name();
    LOG_INFO("service_name:%s", service_name.c_str());
    // 获取服务对象service方法的数量
    int methodCnt = pserviceDesc->method_count();
    for(int i = 0; i < methodCnt; ++i) {
        // 获取服务对象指定下标的服务方法的描述(抽象描述)
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        LOG_INFO("method_name:%s", method_name.c_str());
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

// 启动rpc服务节点开始提供服务
void RpcProvider::Run() {
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    // 绑定连接回调核消息读写回调方法, 分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置muduo 库的线程数量
    server.setThreadNum(4);
    // 启动网络服务

    ZkClient zkCli;
    zkCli.Start();
    // service_name:永久性节点  method_name:临时性节点
    for(auto &sp : m_serviceMap) {
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0, 0);
        for(auto &mp : sp.second.m_methodMap) {
            //  /service_name/method_name
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }



    std::cout << "RpcProvider start service at ip: " << ip << " port :" << port << std::endl;
    server.start();
    m_eventLoop.loop();
}
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if(!conn->connected()) {
        // 和rpc client的连接断开了
        conn->shutdown();
    }
}
/*
    RpcProvider 和 RpcComsumer协商好之间通信用的protobuf数据类型
    service_name method_name args   // 定义proto的message类型，进行数据的序列化和反序列化
*/
// 已建立连接用户的读写回调， 如果远程有一个rpc服务的调用请求， 那么OnMessage方法就会响应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp stp) {
    // 接收的远程调用请求
    std::string recv_buf = buf->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);


    // 根据header_size读取数据头的原始字符流, 反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(rpcHeader.ParseFromString(rpc_header_str)) {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else {
        // 数据头反序列化失败
        std::cout << "rpc_header_str:" << rpc_header_str << "parse error" << std::endl;
    }
    // 解析获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4+header_size, args_size);

    // 打印调试信息
    std::cout << "================================================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===================================================" << std::endl;


    auto it = m_serviceMap.find(service_name);
    // 请求了不存在的服务
    if(it == m_serviceMap.end()) {
        std::cout << service_name << "is not existed!" << std::endl;
        return;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end()) {
        std::cout << service_name << ":" << method_name << "is not exist!" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.m_service;   // 获取service对象
    const google::protobuf::MethodDescriptor *method = mit->second;  // 获取method对象

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)) {
        std::cout << "request parse error, content: " << args_str <<  std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    // new UserService().Login();
    // 给method方法的调用，绑定一个Closure回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);
    service->CallMethod(method, nullptr, request, response, done);

}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message*response) {
    std::string response_str;
    if(response->SerializeToString(&response_str)) {
        // 序列化成功后，通过网络把rpc方法的结果发送给rpc的调用方
        conn->send(response_str);
    }
    else {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown();   // 模拟http的短链接， 有rpcProvider 主动断开连接
}