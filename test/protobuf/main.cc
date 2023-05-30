#include<iostream>
#include "test.pb.h"
using namespace fixbug;
#include<string.h>
int main() {
    /*
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");
    std::string send_str;
    // 序列化
    if(req.SerializeToString(&send_str)) {
        std::cout << send_str << std::endl;
    }
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str)) {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
    */
    /*
    LoginResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("登录处理失败");
    */

    GetFriendListsResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(0);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(23);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;
    std::cout << rsp.mutable_friend_list(0) << std::endl;

    return 0;
}