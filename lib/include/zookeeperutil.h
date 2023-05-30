#pragma once
#define THREADED
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

class ZkClient{
    public:
        ZkClient();
        ~ZkClient();
        // 启动zkclient启动连接zkserver
        void Start();
        // 在zkserver上指定的path创建znode节点
        void Create(const char *path, const char *data, int datalen, int state=0);
        //根据参数指定znode节点路径，获取对应的数据
        std::string GetData(const char *path);
    private:
        zhandle_t *m_zhandle;
};