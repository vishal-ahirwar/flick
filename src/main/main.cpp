#include <main/main.h>
int main(int argc, char *argv[])
{
    if (argc < 2)return solix::showHelp();
    return solix::createApp(::getArgs(argc, argv));
};
