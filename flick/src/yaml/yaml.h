#pragma once
#include <string>
struct TemplateCode {
	std::string CLANGD;
	std::string CLANG_FORMAT;
	std::string CLANG_TIDY;
	std::string EDITOR_CONFIG;
	std::string LICENSE_TEXT;
	std::string CPACK_CODE;
	std::string UNIT_TEST_CODE;
	std::string GITIGNORE_CODE;
	std::string CONFIG_CMAKE;
	std::string MAIN_CODE;
	std::string VIM_CONFIG;
	std::string CMAKE_PRESETS;
	std::string COMPILER_URL;
	std::string CMAKE_URL;
	std::string NINJA_URL;
	std::string VS_BUILD_TOOLS_INSTALLER_URL;
};
class Yaml
{
	TemplateCode mCode;
      public:
	bool init();
      const TemplateCode&getCode()const;
};
