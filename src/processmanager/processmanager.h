#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include <vector>
#include <string_view>
class ProcessManager
{
public:
    static int startProcess(const std::vector<std::string_view> args,bool b_log=false);
};
#endif