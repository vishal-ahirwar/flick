#include <userinfo/userinfo.h>

void UserInfo::readUserInfo() {}

void UserInfo::writeUserInfo() {}
UserInfo::UserInfo() : mUserName{"None"} {}

std::string UserInfo::getUserName() const { return mUserName; }

void UserInfo::readUserInfoFromConfigFile(UserInfo* user) {

};

void UserInfo::writeUserInfoToConfigFile(UserInfo* user) {

};
