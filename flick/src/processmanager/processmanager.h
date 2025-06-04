#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include <vector>
#include <string_view>
class ProcessManager
{
public:
    static int startProcess(const std::vector<std::string>&args,std::string&processLog,const std::string&msg,bool bLog=false);
};
#endif