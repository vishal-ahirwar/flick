#include <deps/libs.h>

Libs::Libs(const std::string& name, const std::string& url, const std::string& cmakeCustomArgs) {}
std::string Libs::getName() const { return std::string(); }
std::string Libs::getUrl() const { return std::string(); }
std::string Libs::getCustomCMakeArgs() const { return std::string(); };
bool Libs::bBuildLocal() const { return mBBuildLocal; };
