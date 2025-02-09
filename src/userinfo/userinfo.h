#ifndef _USER_INFO_H
#define _USER_INFO_H
#include <string>
class UserInfo
{
    std::string _user_name{};
    public:
    UserInfo();
    std::string getUserName()const;
};

#endif