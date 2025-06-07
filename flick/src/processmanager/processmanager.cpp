#include "processmanager.h"
#include "log/log.h"
#include <array>
#include <barkeep/barkeep.h>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;
namespace bk = barkeep;
int ProcessManager::startProcess(const std::vector<std::string>& args, std::string& processLog, const std::string& msg, bool b_log)
{
	std::shared_ptr<barkeep::AnimationDisplay> anim{};
	if (!msg.empty()) {
		if (!b_log)
			anim = bk::Animation(
			  {.message = "\033[32m[+]\033[0m " + msg, .style = bk::AnimationStyle::Ellipsis});
	}
	if (!fs::exists("build")) {
		fs::create_directories("build");
	}
	boost::process::ipstream outStream;
	boost::process::ipstream errStream;
	boost::process::child process(boost::process::search_path(args[0]),
				      boost::process::args(std::vector<std::string>(args.begin() + 1, args.end())),
				      boost::process::std_out > outStream, boost::process::std_err > errStream);
	std::string lineOut, lineErr;
	while (process.running() && (std::getline(outStream, lineOut) || std::getline(errStream, lineErr))) {
		if (!lineErr.empty()) {
			if (lineErr.find("warning") != std::string::npos) {

				puts("");
				Logger::warning(lineErr);
				// puts("");
			}

			else if (lineErr.find("error") != std::string::npos) {
				puts("");
				Logger::error(lineErr);
				// puts("");
			} else if (b_log)
				Logger::status(lineErr);
			processLog.append(lineErr + "\n");
		}
		if (!lineOut.empty()) {
			if (lineOut.find("warning") != std::string::npos) {
				puts("");
				Logger::warning(lineOut);
				// puts("");
			}

			else if (lineOut.find("error") != std::string::npos) {
				puts("");
				Logger::error(lineOut);
				// puts("");
			}

			else if (b_log)
				Logger::status(lineOut);
			processLog.append(lineOut + "\n");
		}
	}
	process.wait();
	int exitCode = process.exit_code();
	if (anim) {
		anim->done();
	}
	std::fstream fs("build/logs.txt", std::ios::out);
	if (!fs.is_open()) {
		Log::log("failed to open compile.logs", Type::E_ERROR);
	} else {
		if (processLog.length() <= 0) {
			processLog.append("[Important Advice] Check Your CMakeLists.txt\n");
			processLog.append("[Important Advice] Check Your vcpkg.json\n");
		}
		fs.write(processLog.c_str(), processLog.length());
		if (exitCode != 0) {
			Log::log("You can find more info in \033[95mbuild/logs.txt\033[0m", Type::E_DISPLAY);
		}
		fs.close();
	}
	return exitCode;
}
