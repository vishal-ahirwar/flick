#ifndef _USER_INFO_H
#define _USER_INFO_H
#include <string>
class UserInfo
{
    std::string _user_name{};

public:
    void readUserInfo();
    void writeUserInfo();
    UserInfo();
    std::string getUserName() const;
    static void askUserinfo(UserInfo *user);
    static void readUserInfoFromConfigFile(UserInfo *user);
    static void writeUserInfoToConfigFile(UserInfo *user);
};

#endif