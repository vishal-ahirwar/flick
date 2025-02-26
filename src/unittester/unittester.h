#ifndef UNITTESTER_H
#define UNITTESTER_H
#include <userinfo/userinfo.h>
class UnitTester
{
    UserInfo _user_info{};

public:
    UnitTester(const UserInfo&);
    void setupUnitTestingFramework();
    void generateLicenceFile();
    void runUnitTesting();
};
#endif // UNITTESTER_H