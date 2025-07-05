#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include <string_view>
#include <vector>
#include <string>
class ProcessManager
{
      public:
	static int startProcess(const std::vector<std::string>& args, std::string& processLog, const std::string& msg, bool bLog = true);
};
#endif