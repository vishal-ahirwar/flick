#include <userinfo/userinfo.h>
#include "userinfo.h"

UserInfo::UserInfo() : _user_name{"None"}
{
}

std::string UserInfo::getUserName() const
{
    return _user_name;
}