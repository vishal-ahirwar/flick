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
        if (args.at(1) == "new")
        {
            flick.createNewProject();
        }else if (args.at(1) == "list")
        {
            flick.listPackages();
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
        else if (args.at(1) == ("init"))
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
        else if (args.at(1) == ("installer"))
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
        else if (args.at(1) == ("clean"))
        {
            flick.reBuild();
        }
        else if (args.at(1) == "install")
        {
            flick.addDeps();
        }
        else if (args.at(1) == "preset")
        {
            flick.genCMakePreset();
        }else if (args.at(1)=="subproject")
        {
            flick.createSubProject();
        }
        else
        {
            Log::log("Invalid Command! try 'flick help' for more info");
            return 0;
        };
        return 0;
    };

}

