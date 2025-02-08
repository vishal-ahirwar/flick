#include <app/app.h>
#include <constants/colors.hpp>
#include <aura/aura.hpp>
#include <auraConfig.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include<log/log.h>
namespace CLI
{
    int showHelp()
    {
        Log::about();
        return 0;
    }
    int createApp(const char *argv[], int argc)
    {
        Aura aura;
        if (std::string(argv[1]) == std::string("create"))
        {
            if (argc < 3)
            {
                printf("[Error]Project name can't be null!\n");
                return 1;
            };

            aura.createNewProject(argv, argc);
        }
        else if (std::string(argv[1]) == std::string("help"))
        {
            CLI::showHelp();
        }
        else if (std::string(argv[1]) == std::string("compile"))
        {
            if (argc > 2)
                aura.compile(std::string(argv[2]));
            else
                aura.compile();
        }
        else if (std::string(argv[1]) == std::string("run"))
        {
            aura.run(argc - 2, argv + 2);
        }
        else if (std::string(argv[1]) == std::string("build"))
        {
            aura.build();
        }
        else if (std::string(argv[1]) == std::string("setup"))
        {
            try
            {
                aura.setup();
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (std::string(argv[1]) == std::string("createinstaller"))
        {
            aura.createInstaller();
        }
        else if (std::string(argv[1]) == std::string("utest"))
        {
            aura.test();
        }
        else if (std::string(argv[1]) == std::string("fix"))
        {
            aura.fixInstallation();
        }
        else if (std::string(argv[1]) == std::string("update"))
        {
            aura.update();
        }
        else if (std::string(argv[1]) == std::string("debug"))
        {
            aura.debug();
        }
        else if (std::string(argv[1]) == std::string("release"))
        {
            aura.release();
        }
        else if(std::string(argv[1])==std::string("vscode"))
        {
            aura.vsCode();
        }else if(std::string(argv[1])==std::string("rebuild"))
        {
            aura.reBuild();
        }
        else
        {
            printf("%s[Error] Invalid Command !%s\n", RED, WHITE);
            printf("Try again with 'aura help' :(\n");
            return 0;
        };
        return 0;
    };

}
