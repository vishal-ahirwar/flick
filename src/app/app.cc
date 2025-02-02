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
        App app;
        if (std::string(argv[1]) == std::string("create"))
        {
            if (argc < 3)
            {
                printf("[Error]Project name can't be null!\n");
                return 1;
            };

            app.createNewProject(argv, argc);
        }
        else if (std::string(argv[1]) == std::string("help"))
        {
            CLI::showHelp();
        }
        else if (std::string(argv[1]) == std::string("compile"))
        {
            if (argc > 2)
                app.compile(std::string(argv[2]));
            else
                app.compile();
        }
        else if (std::string(argv[1]) == std::string("run"))
        {
            app.run(argc - 2, argv + 2);
        }
        else if (std::string(argv[1]) == std::string("build"))
        {
            app.build();
        }
        else if (std::string(argv[1]) == std::string("setup"))
        {
            try
            {
                app.setup();
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (std::string(argv[1]) == std::string("createinstaller"))
        {
            app.createInstaller();
        }
        else if (std::string(argv[1]) == std::string("utest"))
        {
            app.test();
        }
        else if (std::string(argv[1]) == std::string("fix"))
        {
            app.fixInstallation();
        }
        else if (std::string(argv[1]) == std::string("update"))
        {
            app.update();
        }
        else if (std::string(argv[1]) == std::string("debug"))
        {
            app.debug();
        }
        else if (std::string(argv[1]) == std::string("release"))
        {
            app.release();
        }
        else if (std::string(argv[1]) == std::string("add"))
        {
            if (argc < 3)
            {
                printf("%s[Error] Invalid Command !%s\n", RED, WHITE);
                printf("Try again with 'aura help' :(\n");
                return 0;
            };
            app.addConanPackage(argv[2]);
        }
        else if (std::string(argv[1]) == std::string("reload"))
        {
            app.reloadPackages();
        }
        else if (std::string(argv[1]) == std::string("initconan"))
        {
            app.initConan();
        }else if(std::string(argv[1])==std::string("vscode"))
        {
            app.vsCode();
        }else if(std::string(argv[1])==std::string("rebuild"))
        {
            app.reBuild();
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
