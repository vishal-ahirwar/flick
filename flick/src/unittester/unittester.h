#ifndef UNITTESTER_H
#define UNITTESTER_H
#include <userinfo/userinfo.h>
#include<vector>
class UnitTester
{
    UserInfo mUserInfo{};

public:
    UnitTester(const UserInfo &);
    void setupUnitTestingFramework();
    bool runUnitTesting(const std::vector<std::string>&args);
};
#endif // UNITTESTER_H