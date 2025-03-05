#include <projectgenerator/projectgenerator.h>
#include <log/log.h>
#include <filesystem>
#include <deps/deps.h>
#include <fstream>
#include <chrono>
#include <format>
#include <assert.h>
void ProjectGenerator::generate()
{
	generateProject();
}

void ProjectGenerator::setProjectSetting(const ProjectSetting &project_setting, const Lang &lang)
{
	_lang = lang;
	_project_setting = project_setting;
}

void ProjectGenerator::generateProject()
{
	Log::log("Creating directory..", Type::E_DISPLAY);
	std::string cmdString{};
	createDir();
	Log::log("Generating Code for main.cxx and CMakeLists.txt..", Type::E_DISPLAY);
	generateCppTemplateFile();
	generateCmakeFile();
	generateGitIgnoreFile();
	writeProjectSettings(&_project_setting);
	Log::log("happy Coding :)", Type::E_DISPLAY);
}

void ProjectGenerator::readProjectSettings(ProjectSetting *setting)
{
	if (!setting)
		return;
	if (!setting->readConfig())
	{
		Log::log("Failed to read config file", Type::E_ERROR);
		exit(0);
	};
};
void ProjectGenerator::writeProjectSettings(ProjectSetting *setting)
{
	if (!setting)
		return;
	setting->writeConfig(setting->getProjectName() + "/");
	Log::log("Generating config.json", Type::E_DISPLAY);
	Deps deps{};
	deps.getSetting().write(setting->getProjectName());
};

// creating folder structure for project
void ProjectGenerator::createDir()
{
	namespace fs = std::filesystem;
	std::string cmdString{};
	cmdString += _project_setting.getProjectName();
	if (fs::create_directory(cmdString.c_str()))
	{
		cmdString += "/src";
		fs::create_directory(cmdString.c_str());
		auto pos = cmdString.find("/");
		cmdString.replace(pos + 1, cmdString.length() - pos, "res");
		fs::create_directory(cmdString.c_str());
		pos = cmdString.find("/");
		cmdString.replace(pos + 1, cmdString.length() - pos, "external");
		fs::create_directory(cmdString.c_str());
	}
	else
	{
		Log::log("failed to create dir error!", Type::E_ERROR);
		exit(0);
	}
};
//
void ProjectGenerator::generateCppTemplateFile()
{
	std::ofstream file;
	file.open("./" + _project_setting.getProjectName() + "/src/main.cxx", std::ios::out);

	if (file.is_open())
	{
		std::string_view header{"_HEADER_"};
		std::string_view copyright{"_COPYRIGHT_"};
		std::string_view project{"_PROJECT_"};
		std::string_view comment{"@COPYRIGHT"};

		std::string cap("", _project_setting.getProjectName().length());
		std::transform(_project_setting.getProjectName().begin(), _project_setting.getProjectName().end(), cap.begin(), ::toupper);

		auto index = MAIN_CODE.find(header);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, header.length(), ("#include<" + _project_setting.getProjectName() + "config.h>"));

		index = MAIN_CODE.find(copyright);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, copyright.length(), (cap + "_COPYRIGHT"));

		index = MAIN_CODE.find(project);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, project.length(), "\"" + _project_setting.getProjectName() + "\"");

		index = MAIN_CODE.find(comment);
		if (index != std::string::npos)
			MAIN_CODE.replace(index, comment.length(), _user_info.getUserName());

		file << MAIN_CODE;
		file.close();
	}
}

//
void ProjectGenerator::generateCmakeFile()
{
	std::string config{(_project_setting.getProjectName() + "/res/config.h.in")};
	{
		std::ofstream file;
		std::string cap("", _project_setting.getProjectName().length());
		std::transform(_project_setting.getProjectName().begin(), _project_setting.getProjectName().end(), cap.begin(), ::toupper);
		file.open(config, std::ios::out);
		if (file.is_open())
		{
			file << "namespace Project{" << std::endl;
			file << ("const char*const VERSION_STRING=\"@" + _project_setting.getProjectName() + "_VERSION_MAJOR@.@" + _project_setting.getProjectName() + "_VERSION_MINOR@.@" + _project_setting.getProjectName() + "_VERSION_PATCH@\";") << std::endl;
			file << ("const char*const COMPANY_NAME =\"@COMPANY@\";") << std::endl;
			file << ("const char*const COPYRIGHT_STRING= \"@COPYRIGHT@\";") << std::endl;
			file << ("const char*const PROJECT_NAME=\"@PROJECT_NAME@\";") << std::endl;
			file << "}";
			file.close();
		};
	}
	{

		std::ofstream file;
		file.open("./" + _project_setting.getProjectName() + "/CMakeLists.txt", std::ios::out);
		constexpr std::string_view config_in{"@config_in"};
		constexpr std::string_view config_h{"@config_h"};
		constexpr std::string_view comment{"@COPYRIGHT"};
		constexpr std::string_view developer{"@DeveloperName"};
		if (file.is_open())
		{
			constexpr std::string_view _name{"@name"};
			std::string str(CMAKE_CODE);
			auto index = str.find(_name);
			if (index != std::string::npos)
			{
				str.replace(index, _name.size(), _project_setting.getProjectName());
			};
			index = str.find(config_h);
			if (index != std::string::npos)
				str.replace(index, config_h.length(), (_project_setting.getProjectName() + "config.h"));
			index = str.find(config_in);
			if (index != std::string::npos)
				str.replace(index, config_in.length(), "res/config.h.in");
			index = str.find(comment);
			if (index != std::string::npos)
				str.replace(index, comment.length(), _user_info.getUserName());
			index = str.find(developer);
			if (index != std::string::npos)
				str.replace(index, developer.length(), _user_info.getUserName());
			file << str;
			file.close();
		};
	}
}
//
void ProjectGenerator::generateGitIgnoreFile()
{
	std::ofstream file;
	file.open("./" + _project_setting.getProjectName() + "/.gitignore", std::ios::out);
	if (file.is_open())
	{
		file << GITIGNORE_CODE;

		file.close();
	};
};
void ProjectGenerator::generateLicenceFile(const UserInfo &user_info)
{
	std::ofstream out;
	out.open("License.txt", std::ios_base::out);
	if (!out.is_open())
	{
		Log::log("Failed to Generate License.txt, You may need to create License.txt by "
				 "yourself :)%s",
				 Type::E_ERROR);
		return;
	};
	std::string _licence{LICENSE_TEXT};
	// TODO
	constexpr std::string_view year{"@_YEAR_"};
	constexpr std::string_view name{"@_OWNER_"};
	auto index = _licence.find(year);
	if (index != std::string::npos)
		_licence.replace(index, year.length(), std::format("{:%Y}", std::chrono::system_clock::now()));
	index = _licence.find(name);
	if (index != std::string::npos)
		_licence.replace(index, name.length(), user_info.getUserName());
	out << _licence;
	out.close();
}