#include <app/app.h>
#include <constants/colors.hpp>
#include <aura/aura.hpp>
#include <auraConfig.h>
#include <string>
#include <stdio.h>
#include <iostream>

namespace CLI
{
    int showHelp()
    {
        printf(
            "%sAbout:\nLearning C/C++ and tired of creating files and folder and running .cpp,\ncompilling manually again n again\ntry this it does a lot for you \nlike creating full structured project\n"
            "which will have CMake file, good clean folder structure,.gitignore file, and commands to \nfacilitate your most of time learning C++ without creating \n"
            "a new project every time you want to learn a new concept in C/C++ ;)"
            "%s\n\nversion : %d.%d.%d\nAuthor: %s\n%s\n"
            "%s\n[Usage]: \n"
            "aura [command] projectname\n"
            "\n[available commands]%s\n\n"
            "create- create a new C/C++ project [aura create projectName]\n"
            "compile- compile the project[cd into project directory [aura compile <additional cmake args>]] ex. aura compile -DCMAKE_BUILD_TYPE=Debug\n"
            "run- run the compiled binary make sure to compile the project before running this command\n"
            "help-show the help wizard\n"
            "build - compile and run the project with this single command\n"
            "setup - this command will install a g++ compiler and cmake to your system if not already installed\n"
            "utest : to enable unit testing{aura create test --test}\n"
            "createinstaller : to create packged build of your application\n"
            "update : update the aura to latest version\n"
            "add : to add external library to your project using conan package manager ex. aura add fmt/11.0.2\n"
            "reload : to reload the package list ex.aura reload\n"
            "rebuild : this will remove the build directory and compile the project again\n"
            "initconan : to add conan file\n"
            "debug : to start GDB debugger\n"
            "release: to compile application in release mode or you can do this manually aura compile -DCMAKE_BUILD_TYPE=Release\n"
            "vscode : to geenrate vscode c/c++ configuration file\n"
            "fix : to fix the aura installation\n%s",
            BLUE, YELLOW, AURA_VERSION_MAJOR, AURA_VERSION_MINOR, AURA_VERSION_PATCH,
            AURA_COMPANY, AURA_COPYRIGHT, CYAN, GREEN, WHITE);
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
            app.add(argv[2]);
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
            app.vscode();
        }else if(std::string(argv[1])==std::string("rebuild"))
        {
            app.rebuild();
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
