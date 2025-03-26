#include <gtest/gtest.h>
#include<projectgenerator/projectgenerator.h>
TEST(ProjectGenerator,getConfig)
{
    ProjectGenerator project_generator;
    std::string result{};
    EXPECT_EQ(project_generator.getFromConfig("cpp/project/console/cc",result),true);
    EXPECT_EQ(project_generator.getFromConfig("c/project/console/cmake",result),true);
    EXPECT_EQ(project_generator.getFromConfig("cpp/preset",result),true);
    EXPECT_EQ(project_generator.getFromConfig("username",result),true);
    EXPECT_EQ(project_generator.getFromConfig("cpp/test",result),true);
    EXPECT_EQ(project_generator.getFromConfig("c/test",result),true);
    EXPECT_EQ(project_generator.getFromConfig("license",result),true);
    EXPECT_EQ(project_generator.getFromConfig("windows/urls",result),false);
    EXPECT_EQ(project_generator.getFromConfig("urls/windows/compiler",result),true);
    EXPECT_EQ(project_generator.getFromConfig("urls/linux/compiler",result),true);
    EXPECT_EQ(project_generator.getFromConfig("urls/windows/cmake",result),true);
    EXPECT_EQ(project_generator.getFromConfig("urls/windows/ninja",result),true);
    EXPECT_EQ(project_generator.getFromConfig("urls/linux/utool",result),true);
    EXPECT_EQ(project_generator.getFromConfig("urls/windows/vs_build_tools",result),true);
}
TEST(ProcessManager,startProcess)
{

}
TEST(ProjectSetting,readConfig)
{

}
TEST(ProcessSetting,writeConfig)
{

}
TEST(Utils,get)
{

}
TEST(Utils,set)
{

}
TEST(Downloader,download)
{

}
TEST(Deps,read)
{

}
TEST(Deps,write)
{

}
TEST(Utool,start)
{

}
TEST(UnitTester,run)
{

}
TEST(UnitTester,setup)
{

}
TEST(RT,init)
{

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
