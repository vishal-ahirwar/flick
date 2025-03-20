#ifndef UNITTESTER_H
#define UNITTESTER_H
#include <userinfo/userinfo.h>
#include<vector>
class UnitTester
{
    UserInfo _user_info{};

public:
    UnitTester(const UserInfo &);
    void setupUnitTestingFramework();
    void runUnitTesting(const std::vector<std::string>&_args);
};
#endif // UNITTESTER_H