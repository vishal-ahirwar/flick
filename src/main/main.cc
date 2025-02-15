#include <main/main.h>
int main(int argc, char *argv[])
{
    if (argc < 2)return CLI::showHelp();
    return CLI::createApp(Main::getArgs(argc, argv));
};
