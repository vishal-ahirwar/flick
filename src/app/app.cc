#include <app/app.h>
#include <constants/colors.hpp>
#include <solix/solix.hpp>
#include <solixconfig.h>
#include <string>
#include <log/log.h>
namespace solix
{
    int showHelp()
    {
        Log::about();
        return 0;
    }
    int createApp(const std::vector<std::string> &args)
    {
        Solix Solix{args};
        if (args.at(1) == "create")
        {
            Solix.createNewProject();
        }
        else if (args.at(1) == ("help"))
        {
            solix::showHelp();
        }
        else if (args.at(1) == ("compile"))
        {
            Solix.compile();
        }
        else if (args.at(1) == ("run"))
        {
            Solix.run();
        }
        else if (args.at(1) == ("build"))
        {
            Solix.build();
        }
        else if (args.at(1) == ("setup"))
        {
            try
            {
                Solix.setup();
            }
            catch (const std::exception &e)
            {
                Log::log(e.what(), Type::E_ERROR);
            }
        }
        else if (args.at(1) == ("createinstaller"))
        {
            Solix.createInstaller();
        }
        else if (args.at(1) == ("tests"))
        {
            Solix.test();
        }
        else if (args.at(1) == ("doctor"))
        {
            Solix.fixInstallation();
        }
        else if (args.at(1) == ("update"))
        {
            Solix.update();
        }
        else if (args.at(1) == ("debug"))
        {
            Solix.debug();
        }
        else if (args.at(1) == ("release"))
        {
            Solix.release();
        }
        else if (args.at(1) == ("vscode"))
        {
            Solix.vsCode();
        }
        else if (args.at(1) == ("rebuild"))
        {
            Solix.reBuild();
        }
        else if (args.at(1) == "install")
        {
            Solix.addDeps();
        }
        else if (args.at(1) == "cmake-preset")
        {
            Solix.genCMakePreset();
        }else if (args.at(1)=="subproject")
        {
            Solix.createSubProject();
        }
        else
        {
            printf("%s[Error] Invalid Command !%s\n", RED, WHITE);
            printf("Try again with 'Solix help' :(\n");
            return 0;
        };
        return 0;
    };

}
