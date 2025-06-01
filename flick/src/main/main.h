#ifndef MAIN_H
#define MAIN_H
#include <app/app.h>
#include <log/log.h>
namespace 
{
    std::vector<std::string> getArgs(int argc, char *argv[])
    {
        std::vector<std::string> args{};
        for (int i = 0; i < argc; ++i)
            args.push_back(argv[i]);
        return args;
    }
}
#endif