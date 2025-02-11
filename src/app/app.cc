#include <app/app.h>
#include <constants/colors.hpp>
#include <aura/aura.hpp>
#include <auraConfig.h>
#include <string>
#include <log/log.h>
namespace CLI
{
    int showHelp()
    {
        Log::about();
        return 0;
    }
    int createApp(const std::vector<std::string> &args)
    {
        Aura aura{args};
        if (args.at(1) == "create")
        {
            aura.createNewProject();
        }
        else if (args.at(1) == ("help"))
        {
            CLI::showHelp();
        }
        else if (args.at(1) == ("compile"))
        {
            aura.compile();
        }
        else if (args.at(1) == ("run"))
        {
            aura.run();
        }
        else if (args.at(1) == ("build"))
        {
            aura.build();
        }
        else if (args.at(1) == ("setup"))
        {
            try
            {
                aura.setup();
            }
            catch (const std::exception &e)
            {
                Log::log(e.what(), Type::E_ERROR);
            }
        }
        else if (args.at(1) == ("createinstaller"))
        {
            aura.createInstaller();
        }
        else if (args.at(1) == ("utest"))
        {
            aura.test();
        }
        else if (args.at(1) == ("fix"))
        {
            aura.fixInstallation();
        }
        else if (args.at(1) == ("update"))
        {
            aura.update();
        }
        else if (args.at(1) == ("debug"))
        {
            aura.debug();
        }
        else if (args.at(1) == ("release"))
        {
            aura.release();
        }
        else if (args.at(1) == ("vscode"))
        {
            aura.vsCode();
        }
        else if (args.at(1) == ("rebuild"))
        {
            aura.reBuild();
        }
        else if (args.at(1) == "ref")
        {
            aura.refresh();
        }
        else if (args.at(1) == "builddeps")
        {
            aura.buildDeps();
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
