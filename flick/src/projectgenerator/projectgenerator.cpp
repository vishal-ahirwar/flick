#include "projectgenerator.h"
#include <chrono>
#include <deps/deps.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <log/log.h>
#include <projectgenerator/projectgenerator.h>
#include <utils/utils.h>

namespace fs = std::filesystem;
ProjectGenerator::ProjectGenerator() {

};
bool ProjectGenerator::getFromConfig(const std::string& key, std::string& result)
{
	std::vector<std::string> tokens{};
	std::istringstream ss(key);
	std::string token;
	while (std::getline(ss, token, '/')) {
		tokens.push_back(token);
	};
	nlohmann::json json;
	for (const auto& token : tokens) {
		if (json.is_null()) {
			if (!mConfig.contains(token)) {
				return false;
			}
			json = mConfig[token];
		} else {
			if (!json.contains(token)) {
				return false;
			}
			json = json[token];
		}
	};
	if (json.is_string()) {
		result = json.get<std::string>();
	} else {
		Log::log(token + " is not a string", Type::E_ERROR);
		return false;
	}
	return true;
}

bool ProjectGenerator::generateSubProject(const std::string& projectName, bool isRoot)
{
	if (isRoot) {
		configCMake();
		generateRootCMake();
		std::ofstream out(mProjectSetting.getProjectName() + "/CMakePresets.json");
		if (!out.is_open()) {
			Log::log("failed to generate CMakePresets.json", Type::E_ERROR);
			return false;
		};
		out << CMAKE_PRESETS[static_cast<int>(_lang)];
		out.close();
		generateVcpkgFiles();
		generateGitIgnoreFile();
		std::ofstream file{mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/CMakeLists.txt"};
		switch (_lang) {
		case Language::CXX:
			switch (_type) {
			case ProjectType::EXECUTABLE:
				file << fmt::format("add_executable({} src/main.cpp)# Add your Source Files here\n", projectName);
				break;
			case ProjectType::LIBRARY:
				file << fmt::format("add_library({} src/{}.cpp)# Add your Source Files here\n", projectName, projectName);
				break;
			}
			break;
		case Language::C:
			switch (_type) {
			case ProjectType::EXECUTABLE:
				file << fmt::format("add_executable({}  src/main.c)# Add your Source Files here\n", projectName);
				break;
			case ProjectType::LIBRARY:
				file << fmt::format("add_library({}  src/{}.c)# Add your Source Files here\n", projectName, projectName);
				break;
			}
			break;
		};

		file.close();
	} else {
		fs::create_directories(projectName + "/src");
		fs::create_directories(projectName + "/include");
		generateSubProjectCMake(projectName);
	};

	generateCppTemplateFile(projectName, isRoot);

	std::vector<std::string> lines{};
	std::string line{};
	std::ifstream in{};
	isRoot ? in.open(mProjectSetting.getProjectName() + "/CMakeLists.txt") : in.open("CMakeLists.txt");
	size_t index = 0, appnedIndex = 0;
	while (std::getline(in, line)) {
		++index;
		lines.push_back(line);
		if (line.find("@add_subproject") != std::string::npos)
			appnedIndex = index;
	};
	in.close();
	lines.insert(lines.begin() + appnedIndex, fmt::format("add_subdirectory({})", projectName));
	std::ofstream out;
	isRoot ? out.open(mProjectSetting.getProjectName() + "/CMakeLists.txt") : out.open("CMakeLists.txt");
	for (const auto& line : lines)
		out << line << "\n";
	out.close();
	return true;
}

void ProjectGenerator::generate() { generateProject(); }

void ProjectGenerator::setProjectSetting(const ProjectSetting& projectSetting, const Language& lang, const ProjectType& type)
{
	_lang = lang;
	_type = type;
	mProjectSetting = projectSetting;
}

void ProjectGenerator::generateProject()
{
	Log::log("Creating directory..", Type::E_DISPLAY);
	std::string cmdString{};
	createDir();
	if (_lang == Language::C) {

		Log::log("Generating Starter Project \033[95mC\033[0m language", Type::E_DISPLAY);
	}
	if (_lang == Language::CXX) {

		Log::log("Generating Starter Project \033[95mC++\033[0m language", Type::E_DISPLAY);
	}
	if (_type == ProjectType::EXECUTABLE) {

		Log::log("Project Type is \033[95mExecutable\033[0m", Type::E_DISPLAY);
	}
	if (_type == ProjectType::LIBRARY) {

		Log::log("Project Type is \033[95mLibrary\033[0m", Type::E_DISPLAY);
	}
	generateSubProject(mProjectSetting.getProjectName(), true);
	Log::log("The compiler is silent... for now.", Type::E_WARNING);
	writeProjectSettings(&mProjectSetting);
}

void ProjectGenerator::generateVcpkgFiles()
{
	std::ofstream out(mProjectSetting.getProjectName() + "/CMakePresets.json");
	if (!out.is_open()) {
		Log::log("failed to generate CMakePresets.json", Type::E_ERROR);
		return;
	};
	out << CMAKE_PRESETS[static_cast<int>(_lang)];
	out.close();
}
void ProjectGenerator::generateRootCMake()
{
	std::string config{(mProjectSetting.getProjectName() + "/res/config.h.in")};
	std::ofstream cmake{mProjectSetting.getProjectName() + "/CMakeLists.txt"};
	cmake << "#Auto Generated Root CMake file by Flick CLI\n";
	cmake << fmt::format("#Copyright(c) 2025 {}.All rights reerved.\n", mUserInfo.getUserName());
	cmake << "cmake_minimum_required(VERSION 3.6...3.31)\n";
	if (_lang == Language::CXX) {
		std::ofstream file;
		std::string cap("", mProjectSetting.getProjectName().length());
		std::transform(mProjectSetting.getProjectName().begin(), mProjectSetting.getProjectName().end(), cap.begin(), ::toupper);
		file.open(config, std::ios::out);
		if (file.is_open()) {
			file << "#pragma once\n";
			file << "#include<string_view>" << std::endl;
			file << "namespace Project{" << std::endl;
			file << ("\tconstexpr std::string_view VERSION_STRING=\"@" + mProjectSetting.getProjectName() + "_VERSION_MAJOR@.@" +
				 mProjectSetting.getProjectName() + "_VERSION_MINOR@.@" + mProjectSetting.getProjectName() + "_VERSION_PATCH@\";")
			     << std::endl;
			file << ("\tconstexpr std::string_view COMPANY_NAME =\"@COMPANY@\";") << std::endl;
			file << ("\tconstexpr std::string_view COPYRIGHT_STRING= \"@COPYRIGHT@\";") << std::endl;
			file << ("\tconstexpr std::string_view PROJECT_NAME=\"@PROJECT_NAME@\";") << std::endl;
			file << "}";
			file.close();
		};
		cmake << fmt::format("project({} VERSION 1.0.0 LANGUAGES CXX)\n", mProjectSetting.getProjectName());
	} else if (_lang == Language::C) {
		std::ofstream file;
		std::string cap("", mProjectSetting.getProjectName().length());
		std::transform(mProjectSetting.getProjectName().begin(), mProjectSetting.getProjectName().end(), cap.begin(), ::toupper);
		file.open(config, std::ios::out);
		if (file.is_open()) {
			file << "#pragma once\n";
			file << ("const char*const VERSION_STRING=\"@" + mProjectSetting.getProjectName() + "_VERSION_MAJOR@.@" +
				 mProjectSetting.getProjectName() + "_VERSION_MINOR@.@" + mProjectSetting.getProjectName() + "_VERSION_PATCH@\";")
			     << std::endl;
			file << ("const char*const COMPANY_NAME =\"@COMPANY@\";") << std::endl;
			file << ("const char*const COPYRIGHT_STRING= \"@COPYRIGHT@\";") << std::endl;
			file << ("const char*const PROJECT_NAME=\"@PROJECT_NAME@\";") << std::endl;
			file.close();
		};
		cmake << fmt::format("project({} VERSION 1.0.0 LANGUAGES C)\n", mProjectSetting.getProjectName());
	}
	cmake << "include(cmake/config.cmake)\n";
	cmake << "#@add_find_package Warning: Do not remove this line\n";
	cmake << "#@add_subproject Warning: Do not remove this line\n";
	cmake.close();
}
void ProjectGenerator::generateSubProjectCMake(const std::string& projectName)
{
	if (fs::exists(projectName + "/CMakeLists.txt"))
		return;
	std::ofstream file{projectName + "/CMakeLists.txt"};
	switch (_lang) {
	case Language::CXX:
		switch (_type) {
		case ProjectType::EXECUTABLE:
			file << fmt::format("add_executable({} src/main.cpp)# Add your Source Files here\n", projectName);
			break;
		case ProjectType::LIBRARY:
			file << fmt::format("add_library({} src/{}.cpp)# Add your Source Files here\n", projectName, projectName);
			break;
		}
		break;
	case Language::C:
		switch (_type) {
		case ProjectType::EXECUTABLE:
			file << fmt::format("add_executable({}  src/main.c)# Add your Source Files here\n", projectName);
			break;
		case ProjectType::LIBRARY:
			file << fmt::format("add_library({}  src/{}.c)# Add your Source Files here\n", projectName, projectName);
			break;
		}
		break;
	};

	file.close();
}
void ProjectGenerator::configCMake()
{
	std::ofstream file{mProjectSetting.getProjectName() + "/cmake/config.cmake"};
	constexpr std::string_view config_in{"@config_in"};
	constexpr std::string_view config_h{"@config_h"};
	constexpr std::string_view comment{"@COPYRIGHT"};
	constexpr std::string_view developer{"@DeveloperName"};
	if (file.is_open()) {
		constexpr std::string_view _name{"@name"};
		std::string str(CONFIG_CMAKE[static_cast<int>(_lang)]);
		auto index = str.find(_name);
		if (index != std::string::npos) {
			str.replace(index, _name.size(), mProjectSetting.getProjectName());
		};
		index = str.find(config_h);
		if (index != std::string::npos)
			str.replace(index, config_h.length(), (mProjectSetting.getProjectName() + "config.h"));
		index = str.find(config_in);
		if (index != std::string::npos)
			str.replace(index, config_in.length(), "res/config.h.in");
		index = str.find(comment);
		if (index != std::string::npos)
			str.replace(index, comment.length(), mUserInfo.getUserName());
		index = str.find(developer);
		if (index != std::string::npos)
			str.replace(index, developer.length(), mUserInfo.getUserName());
		file << str;
		file.close();
	};
};

void ProjectGenerator::readProjectSettings(ProjectSetting* setting)
{
	if (!setting)
		return;
	if (!setting->readConfig()) {
		Log::log("Failed to read vcpkg.json file", Type::E_ERROR);
		exit(0);
	};
};
void ProjectGenerator::writeProjectSettings(ProjectSetting* setting)
{
	if (!setting)
		return;
	setting->writeConfig(setting->getProjectName() + "/");
};

void ProjectGenerator::generateCMakePreset(const Language& lang)
{
	std::ofstream out("CMakePresets.json");

	if (!out.is_open()) {
		Log::log("failed to generate CMakePresets.json", Type::E_ERROR);
		return;
	};
	out << CMAKE_PRESETS[static_cast<int>(lang)];
	out.close();
}
// creating folder structure for project
void ProjectGenerator::createDir()
{
	namespace fs = std::filesystem;
	fs::create_directories(mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/src");
	fs::create_directories(mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/include");
	fs::create_directories(mProjectSetting.getProjectName() + "/res");
	fs::create_directories(mProjectSetting.getProjectName() + "/cmake");
};
//
void ProjectGenerator::generateCppTemplateFile(const std::string& projectName, bool isRoot)
{
	std::ofstream sourceFile, headerFile;
	if (_lang == Language::CXX) {
		switch (_type) {
		case ProjectType::EXECUTABLE:
			isRoot ? sourceFile.open("./" + mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/src/main.cpp",
						 std::ios::out)
			       : sourceFile.open("./" + projectName + "/src/main.cpp", std::ios::out);
			break;
		case ProjectType::LIBRARY:
			isRoot ? sourceFile.open("./" + mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/src/" +
						   mProjectSetting.getProjectName() + ".cpp",
						 std::ios::out)
			       : sourceFile.open("./" + projectName + "/src/" + projectName + ".cpp", std::ios::out);
			isRoot ? headerFile.open("./" + mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/include/" +
						   mProjectSetting.getProjectName() + ".hpp",
						 std::ios::out)
			       : headerFile.open("./" + projectName + "/include/" + projectName + ".hpp", std::ios::out);
			break;
		}
	} else if (_lang == Language::C) {
		switch (_type) {
		case ProjectType::EXECUTABLE:
			isRoot ? sourceFile.open("./" + mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/src/main.c",
						 std::ios::out)
			       : sourceFile.open("./" + projectName + "/src/main.c", std::ios::out);
			break;
		case ProjectType::LIBRARY:
			isRoot ? sourceFile.open("./" + mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/src/" +
						   mProjectSetting.getProjectName() + ".c",
						 std::ios::out)
			       : sourceFile.open("./" + projectName + "/src/" + projectName + ".c", std::ios::out);
			isRoot ? headerFile.open("./" + mProjectSetting.getProjectName() + "/" + mProjectSetting.getProjectName() + "/include/" +
						   mProjectSetting.getProjectName() + ".h",
						 std::ios::out)
			       : headerFile.open("./" + projectName + "/include/" + projectName + ".h", std::ios::out);
			break;
		}
	}
	if (sourceFile.is_open()) {
		switch (_type) {
		case ProjectType::EXECUTABLE:
			[&]() {
				std::string_view header{"_HEADER_"};
				std::string_view copyright{"_COPYRIGHT_"};
				std::string_view project{"_PROJECT_"};
				std::string_view comment{"@COPYRIGHT"};

				std::string cap("", mProjectSetting.getProjectName().length());
				std::transform(mProjectSetting.getProjectName().begin(), mProjectSetting.getProjectName().end(), cap.begin(),
					       ::toupper);

				auto index = MAIN_CODE[static_cast<int>(_lang)].find(header);
				if (index != std::string::npos)
					MAIN_CODE[static_cast<int>(_lang)].replace(index, header.length(),
										   ("#include<" + mProjectSetting.getProjectName() + "config.h>"));

				index = MAIN_CODE[static_cast<int>(_lang)].find(copyright);
				if (index != std::string::npos)
					MAIN_CODE[static_cast<int>(_lang)].replace(index, copyright.length(), (cap + "_COPYRIGHT"));

				index = MAIN_CODE[static_cast<int>(_lang)].find(project);
				if (index != std::string::npos)
					MAIN_CODE[static_cast<int>(_lang)].replace(index, project.length(),
										   "\"" + mProjectSetting.getProjectName() + "\"");

				index = MAIN_CODE[static_cast<int>(_lang)].find(comment);
				if (index != std::string::npos)
					MAIN_CODE[static_cast<int>(_lang)].replace(index, comment.length(), mUserInfo.getUserName());

				sourceFile << MAIN_CODE[static_cast<int>(_lang)];
			}();
			break;
		case ProjectType::LIBRARY:
			[&]() {
				sourceFile << "#include<" << projectName << "/include/" << projectName << (_lang == Language::C ? ".h" : ".hpp")
					   << ">\n";
				sourceFile << "int callMe(int x){return x*x;}\n";
				std::string headerGuardText{};
				std::transform(projectName.begin(), projectName.end(), std::back_inserter(headerGuardText), ::toupper);
				headerFile << "#ifndef __" << headerGuardText << "__\n";
				headerFile << "#define __" << headerGuardText << "__\n";
				headerFile << "int callMe(int x);\n";
				headerFile << "#endif //" << "__" << headerGuardText << "__\n";
			}();
			break;
		}

		sourceFile.close();
		if (headerFile.is_open())
			headerFile.close();
	}
}

//
void ProjectGenerator::generateGitIgnoreFile()
{
	std::ofstream file;
	file.open("./" + mProjectSetting.getProjectName() + "/.gitignore", std::ios::out);
	if (file.is_open()) {
		file << GITIGNORE_CODE;

		file.close();
	};
};
void ProjectGenerator::generateLicenceFile(const UserInfo& user_info)
{
	std::ofstream out;
	out.open("License.txt", std::ios_base::out);
	if (!out.is_open()) {
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
	if (index != std::string::npos) {
		std::ostringstream oss;
		std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm tm = *std::localtime(&now_c);
		oss << (1900 + tm.tm_year);
		_licence.replace(index, year.length(), oss.str());
	}
	index = _licence.find(name);
	if (index != std::string::npos)
		_licence.replace(index, name.length(), user_info.getUserName());
	out << _licence;
	out.close();
}
