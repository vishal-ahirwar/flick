#include "deps.h"

#include <barkeep/barkeep.h>
#define WIN32_LEAN_AND_MEAN
#include <boost/process.hpp>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <sstream>
#include <thread>

#include "extractor.h"
#include "log/log.h"
#include "processmanager/processmanager.h"

namespace bk = barkeep;
namespace fs = std::filesystem;
bool Deps::buildDeps() { return true; }
const std::string VCPKG_ROOT{
    std::getenv("VCPKG_ROOT") ? std::getenv("VCPKG_ROOT") : ""};
bool Deps::addDeps(const std::string& packageName, const std::string& version,
                   bool forceUpdateBaseLine) {
  std::string _version{};
  std::string name{};
  if (!isPackageAvailableOnVCPKG(packageName, name, _version)) return false;
  if (!version.empty()) {
    if (_version != version) {
      Log::log(fmt::format("Package version : {} does not match", version),
               Type::E_ERROR);
      Log::log("Do you still want to add ?(y/n) ", Type::E_DISPLAY, "");
      char y{};
      std::cin >> y;
      if (y == 'y')
        _version = version;
      else
        return false;
    } else {
      _version = version;
    }
  }
  if (name.empty() || _version.empty()) {
    Log::log("Could not find package!", Type::E_ERROR);
    return false;
  }
  std::string processLog{};
  std::ifstream in("vcpkg.json");
  if (!in.is_open()) return false;
  nlohmann::json data;
  data << in;
  in.close();
  if (data.contains("overrides") && data["overrides"].is_array()) {
    bool found = false;
    for (auto& overrideEntry : data["overrides"]) {
      if (overrideEntry.contains("name") && overrideEntry["name"] == name) {
        overrideEntry["version"] = _version;
        found = true;
        break;
      }
    }
    if (!found) {
      data["overrides"].push_back({{"name", name}, {"version", _version}});
    }
  } else {
    // Create "overrides" array if it doesn't exist
    data["overrides"] = nlohmann::json::array();
    data["overrides"].push_back({{"name", name}, {"version", _version}});
  }

  if (data.contains("builtin-baseline") == false || forceUpdateBaseLine) {
    std::string baseLine{};
    findBuildinBaseline(name, _version, baseLine);
    if (baseLine.empty()) {
      Log::log(
          fmt::format("Built-in baseline not found for version : {}", _version),
          Type::E_ERROR);
      return false;
    }
    data["builtin-baseline"] = baseLine;
  };
  if (data.contains("dependencies")) {
    bool is_package_already_there{};
    for (const auto& package : data["dependencies"]) {
      if (package == packageName) {
        is_package_already_there = true;
        break;
      }
    }
    if (!is_package_already_there) {
      data["dependencies"].emplace_back(packageName);
    }
  }
  std::ofstream out("vcpkg.json");
  if (!out.is_open()) return false;
  out << data.dump(4);
  out.close();
  if (VCPKG_ROOT.empty()) {
    Log::log("VCPKG_ROOT is not set!", Type::E_ERROR);
    return false;
  };
  return true;
};

bool Deps::updateCMakeFile(const std::string& vcpkgLog,
                           const std::string& projectName,
                           const std::string& packageName) {
  std::fstream subProjectCMake(projectName + "/CMakeLists.txt", std::ios::in);
  std::fstream rootCMake("CMakeLists.txt", std::ios::in);
  if (!rootCMake.is_open()) {
    Log::log("Failed to open CMakeLists.txt for Reading", Type::E_ERROR);
    return false;
  };
  if (!subProjectCMake.is_open()) {
    Log::log(fmt::format("Failed to open {}/CMakeLists.txt for Reading",
                         projectName),
             Type::E_ERROR);
    return false;
  }
  std::vector<std::string> rootCMakeLines{}, subProjectLines;
  std::string line{};
  while (std::getline(
      rootCMake,
      line))  // reading whole file in vector to easily update the file
  {
    rootCMakeLines.push_back(line);
  };
  while (std::getline(subProjectCMake, line)) {
    subProjectLines.push_back(line);
  };

  rootCMake.close();
  subProjectCMake.close();
  Extractor extractor;
  auto out = extractor.extract(vcpkgLog, packageName);
  if (out.empty()) {
    Log::log("Extraction failed!", Type::E_ERROR);
    return false;
  };
  auto packages{extractor.getPackages()};
  Log::log("Packages being used in your current project", Type::E_DISPLAY);
  for (auto& [name, values] : packages) {
    if (name.empty()) continue;
    Log::log(fmt::format("\033[95m-----------+{} \033[0m", name), Type::E_NONE);
  }
  packages.clear();
  packages = out;
  for (auto& [name, values] : packages) {
    for (auto& package : values) {
      bool shouldAdd{true};
      for (const auto& line : rootCMakeLines) {
        if (line.find(package) != std::string::npos) {
          shouldAdd = false;
        }
      }
      if (shouldAdd) {
        if (package.find("find") != std::string::npos) {
          for (size_t i = 0; i < rootCMakeLines.size(); ++i) {
            if (rootCMakeLines.at(i).find("@add_find_package") !=
                std::string::npos) {
              rootCMakeLines.insert(rootCMakeLines.begin() + i + 1, package);
              break;
            }
          }
        } else {
          auto index = package.find("main");
          if (index == std::string::npos) continue;
          package.replace(index, strlen("main"), projectName);
          if (std::find(subProjectLines.begin(), subProjectLines.end(),
                        package) != subProjectLines.end())
            continue;
          subProjectLines.push_back(package);
        }
      }
    }
  }
  rootCMake.open("CMakeLists.txt", std::ios::out);
  subProjectCMake.open(projectName + "/CMakeLists.txt", std::ios::out);
  if (!rootCMake.is_open()) {
    Log::log("Failed to open CMakeLists.txt for Writing", Type::E_ERROR);
    return false;
  };
  if (!subProjectCMake.is_open()) {
    Log::log(fmt::format("Failed to open {}/CMakeLists.txt for Writing",
                         projectName),
             Type::E_ERROR);
    return false;
  }
  for (const auto& line : rootCMakeLines) rootCMake << line << "\n";
  for (const auto& line : subProjectLines) subProjectCMake << line << "\n";
  rootCMake.close();
  subProjectCMake.close();
  return true;
}

bool Deps::installDeps(std::string& vcpkgLog, const std::string_view& TRIPLET) {
  std::vector<std::string> args{};
  args.push_back("cmake");
  args.push_back("--preset");
  args.push_back(std::string(TRIPLET));
  return ProcessManager::startProcess(
             args, vcpkgLog,
             "Installing Remaining packages this may take a while") == 0;
}
[[deprecated("Will be removed in future")]]
void Deps::findCMakeConfig(const std::string& root,
                           std::vector<std::string>& configs) {
  if (!fs::exists(root + "/build/install/lib/cmake")) {
    configs.push_back(root);
    return;
  }
  for (const auto& lib_path :
       fs::directory_iterator(root + "/build/install/lib/cmake")) {
    if (lib_path.is_directory()) {
      configs.push_back(lib_path.path().filename().string());
    };
  };
};

bool Deps::isPackageAvailableOnVCPKG(const std::string& packageName,
                                     std::string& outName,
                                     std::string& outVersion) {
  auto anim = bk::Animation(
      {.message = "\033[32m[+]\033[0m Searching for package : " + packageName +
                  "\033[32m",
       .style = bk::AnimationStyle::Ellipsis,
       .interval = 0.1});
  std::vector<std::string> args{"search", packageName, "--classic"};
  boost::process::ipstream out;
  boost::process::ipstream err;
  std::vector<std::string> similiarPackages{};
  if (VCPKG_ROOT.empty()) {
    Log::log("VCPKG_ROOT is not set!", Type::E_ERROR);
    return false;
  };
  try {
#ifdef _WIN32
    std::string extention{"\\vcpkg.exe"};
#else
    std::string extention{"/vcpkg"};
#endif
    boost::process::child c(VCPKG_ROOT + extention, args,
                            boost::process::std_err > err,
                            boost::process::std_out > out);
    std::string line{};
    std::regex pattern(
        R"(^\s*([a-zA-Z0-9\-_]+)\s+((?:\d{4}-\d{2}-\d{2}|\d+(?:\.\d+){0,2})(?:#\d+)?)(?:\s{2,}(.*))?$)");
    while (std::getline(out, line) || std::getline(err, line)) {
      std::smatch match{};
      if (std::regex_match(line, match, pattern)) {
        if (match.size() < 3) {
          Log::log("Failed to parse vcpkg search output", Type::E_ERROR);
          if (anim) anim->done();
          return false;
        } else if (match[1] != packageName && match.size() >= 2) {
          similiarPackages.push_back(match[1]);
          continue;
        }
        outName = match[1];
        outVersion = match[2];
        puts("");
        Log::log(fmt::format("Selected Package : {}, Version : {}, About : {}",
                             outName, outVersion, match[3].str()),
                 Type::E_WARNING);
        c.terminate();
        if (anim) anim->done();
        return true;
      }
    }
  } catch (std::exception& e) {
    if (anim) anim->done();
    Log::log(fmt::format("[Search Package]Exception : {}", e.what()),
             Type::E_ERROR);
    return false;
  }
  if (outName.empty()) {
    puts("");
    Log::log("Package not found!", Type::E_ERROR);
    if (similiarPackages.size() > 0) {
      Log::log("Did you mean one of these packages ?", Type::E_NONE);
      for (const auto& package : similiarPackages) {
        Log::log(fmt::format("\033[95m------{} \033[0m", package));
      }
    }
  }
  if (anim) anim->done();
  return false;
}

bool Deps::findBuildinBaseline(const std::string& name,
                               const std::string& version,
                               std::string& outBaseLine) {
  std::string vcpkg{std::getenv("VCPKG_ROOT")};
  if (vcpkg.length() <= 0) {
    Log::log("VCPKG_ROOT is not Set!", Type::E_ERROR);
    return false;
  }
  boost::process::ipstream out;
  boost::process::ipstream err;
  boost::process::child c(
      boost::process::search_path("git"),
      boost::process::args(
          {"-C", vcpkg, "log", "--format=%H %cd %s", "--date=short", "--",
           std::string("versions/") + name[0] + "-/" + name + ".json"}),
      boost::process::std_err > err, boost::process::std_out > out);
  std::string line{}, first{};
  std::stringstream ss(name);
  std::getline(ss, first, '-');
  std::string latestBaseLine{};
  while (std::getline(out, line) || std::getline(err, line)) {
    if (version.length() <= 0) {
      if (line.find(first) != std::string::npos) {
        auto index = line.find(" ");
        outBaseLine = line.substr(0, index);
        std::string _version = line.substr(index, line.find(" ", index));
        Log::log(fmt::format("Package : {}, Version : {}, Baseline Commit : {}",
                             name, _version, outBaseLine),
                 Type::E_NONE);
        c.terminate();
        return true;
      }
    } else if (line.find(first) != std::string::npos &&
               line.find(version) != std::string::npos) {
      outBaseLine = line.substr(0, line.find(" "));
      Log::log(fmt::format("Package : {}, Version : {}, Baseline Commit : {}",
                           name, version, outBaseLine),
               Type::E_NONE);
      c.terminate();
      return true;
    } else if (latestBaseLine.empty()) {
      latestBaseLine = line.substr(0, line.find(" "));
    }
  }
  c.wait();
  if (outBaseLine.empty()) {
    Log::log(fmt::format("Failed to find Baseline Commit for version : {} "
                         "using latest baseline commit {}",
                         version, latestBaseLine),
             Type::E_WARNING);
    outBaseLine = latestBaseLine;
  }
  return true;
}