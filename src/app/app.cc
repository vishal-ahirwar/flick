#include <app/app.h>
#include <constants/colors.hpp>
#include <flick/flick.hpp>
#include <flickconfig.h>
#include <string>
#include <log/log.h>
namespace flick
{
    int showHelp()
    {
        Log::about();
        return 0;
    }
    int createApp(const std::vector<std::string> &args)
    {
        Flick flick{args};
        if (args.at(1) == "create")
        {
            flick.createNewProject();
        }
        else if (args.at(1) == ("help"))
        {
            flick::showHelp();
        }
        else if (args.at(1) == ("compile"))
        {
            flick.compile();
        }
        else if (args.at(1) == ("run"))
        {
            flick.run();
        }
        else if (args.at(1) == ("build"))
        {
            flick.build();
        }
        else if (args.at(1) == ("setup"))
        {
            try
            {
                flick.setup();
            }
            catch (const std::exception &e)
            {
                Log::log(e.what(), Type::E_ERROR);
            }
        }
        else if (args.at(1) == ("createinstaller"))
        {
            flick.createInstaller();
        }
        else if (args.at(1) == ("tests"))
        {
            flick.test();
        }
        else if (args.at(1) == ("doctor"))
        {
            flick.fixInstallation();
        }
        else if (args.at(1) == ("update"))
        {
            flick.update();
        }
        else if (args.at(1) == ("debug"))
        {
            flick.debug();
        }
        else if (args.at(1) == ("release"))
        {
            flick.release();
        }
        else if (args.at(1) == ("vscode"))
        {
            flick.vsCode();
        }
        else if (args.at(1) == ("rebuild"))
        {
            flick.reBuild();
        }
        else if (args.at(1) == "install")
        {
            flick.addDeps();
        }
        else if (args.at(1) == "cmake-preset")
        {
            flick.genCMakePreset();
        }else if (args.at(1)=="subproject")
        {
            flick.createSubProject();
        }
        else
        {
            printf("%s[Error] Invalid Command !%s\n", RED, WHITE);
            printf("Try again with 'Flick help' :(\n");
            return 0;
        };
        return 0;
    };

}

