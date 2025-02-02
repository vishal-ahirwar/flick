#include<app/app.h>
#include<log/log.h>
int main(int argc, char *argv[])
{
    if (argc < 2)return CLI::showHelp();
    return CLI::createApp((const char **)argv, argc);
};
