#include <userinfo/userinfo.h>

void UserInfo::readUserInfo()
{
}

void UserInfo::writeUserInfo()
{
}
UserInfo::UserInfo() : _user_name{"None"}
{
}

std::string UserInfo::getUserName() const
{
    return _user_name;
}


void UserInfo::readUserInfoFromConfigFile(UserInfo *user) {

};

void UserInfo::writeUserInfoToConfigFile(UserInfo *user) {

};
