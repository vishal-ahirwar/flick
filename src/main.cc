#include <app/app.h>
#include <log/log.h>
int main(int argc, char *argv[])
{
    if (argc < 2)return CLI::showHelp();
    std::vector<std::string> args{};
    for (int i = 0; i < argc; ++i)args.push_back(argv[i]);
    return CLI::createApp(args);
};
