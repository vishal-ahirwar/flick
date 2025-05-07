<<<<<<< HEAD
---
# Solix
[![License: BSD-3-Clause](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
**Solix** is a powerful tool for C/C++ learners, designed to streamline the process of setting up and managing projects. With Solix, you can focus on learning and coding instead of dealing with repetitive tasks like creating folders, writing CMake files, or compiling manually.
=======
>>>>>>> e857387e537ffb402c47b1a421c63fd413504949

# Solix - A Modern C++ Project Setup & Build System

Solix is a fast, reliable, and cross-platform build system for C++ projects. It automates common tasks, like integrating external libraries, managing CMake configuration, and providing an efficient build system to save time for developers.

<<<<<<< HEAD
Learning C/C++ and tired of manually managing files, folders, and compilation? Solix automates these tasks by:
=======
## 🚀 Why Use Solix?
>>>>>>> e857387e537ffb402c47b1a421c63fd413504949

- **Modern C++ Integration**: Easily use external libraries just like you would in other modern languages.
- **Faster Builds**: Enjoy a smoother, more efficient build process with minimal overhead.
- **Consistent Builds Across Platforms**: Whether you're on Windows, Linux, or macOS, Solix ensures consistent builds.
- **Cross-Compiling**: Simplifies the setup for cross-compiling projects across different environments.
- **Save Time on Configuration**: No need to manually write or configure your CMake files every time.
- **No More Starting from Scratch**: Kickstart projects quickly with predefined structures and configurations.

## 🌟 Key Features

- **Automated Project Setup**: Solix sets up a professional-grade project structure, including necessary files like `CMakeLists.txt`, `README.md`, and `.gitignore`, automatically.
- **Effortless External Library Integration**: Easily add external libraries to your project through `vcpkg` or other methods.
- **Cross-Platform Support**: Works seamlessly across major platforms (Windows, Linux, macOS).
- **Fast and Consistent Builds**: Build your projects faster and more reliably with Solix’s pre-configured build system.
- **Cross-Compiling Made Simple**: Set up cross-compilation with ease to target multiple platforms.
- **No More CMake Headaches**: Avoid repetitive CMake configurations by using the default `CMakePresets.json` and build commands.
- **Simplified Compilation**: Solix provides an easy way to compile and run your project with minimal setup.
- **Zero Python Dependency**: Forget about Python-based build systems; Solix is all C++ and CMake-based.

## 🛠️ How to Build Solix from Source

<<<<<<< HEAD
- **Automated Project Setup**: Quickly generate a new C/C++ project structure.
- **Compilation Commands**: Simplified commands for compiling and running code.
- **Integrated Unit Testing**: Easily set up and run unit tests.
- **External Library Support**: Add libraries using VCPKG package manager.
- **Build Automation**: A single command to compile and run your project.
- **Cross-Platform Support**: Works on Windows and Linux.
- **Package Management**: Manage dependencies with ease.
- **GDB Debugger Integration**: Debug your projects seamlessly.
- **Installer Creation**: Generate packaged builds for distribution.
- **Update & Fix Commands**: Keep Solix up-to-date and resolve installation issues.
=======
The only way to build **Solix** from source is with CMake. Follow these simple steps:
>>>>>>> e857387e537ffb402c47b1a421c63fd413504949

### 1. Clone the Repository
```bash
git clone https://github.com/vishal-ahirwar/solix.git
cd solix
```

### 2. Use the Predefined CMake Preset
```bash
cmake --preset=default
cmake --build build/default
```

### Or Use the Built-In Command to Build:
```bash
aura build
```

**Note:** You can also use the command-line options for building and compiling your project with `solix`.

## 📝 Example Usage

### Create a New C++ Project
```bash
solix create myProject
```

### Compile Your Project
```bash
cd myProject
solix build
```

### Add External Library
```bash
solix add fmt/11.0.2
```

### Run the Project
```bash
solix run
```

### Set Up Unit Testing
```bash
solix tests
```

### Create a Packaged Build
```bash
solix createinstaller
```

## 📋 Commands Overview

| Command          | Description                                               |
|------------------|-----------------------------------------------------------|
| `create`         | Create a new C++ project structure.                       |
| `build`          | Compile and build the project.                            |
| `run`            | Run the compiled project.                                 |
| `add`            | Add an external library to the project.                   |
| `tests`          | Enable unit testing for the project.                      |
| `createinstaller`| Generate a packaged installer for the application.        |
| `update`         | Update Solix to the latest version.                       |
| `fix`            | Fix any installation issues.                             |

## ⚙️ Prerequisites

Before using **Solix**, make sure you have the following tools installed:

- **CMake**: Make sure it's installed for building the project.
- **Clang**: Solix uses Clang as the default compiler on all platforms.
- **vcpkg**: For managing external libraries.

## 🔧 Installation

### Install Solix
To install **Solix**, follow these steps:

1. Clone the repository:
   ```bash
<<<<<<< HEAD
   git clone https://github.com/vishal-ahirwar/Solix.git
   cd Solix
=======
   git clone https://github.com/vishal-ahirwar/solix.git
   cd solix
>>>>>>> e857387e537ffb402c47b1a421c63fd413504949
   ```

2. Build and install:
   ```bash
   cmake --preset=default
   cmake --build build/default
   ```

## 📜 License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/vishal-ahirwar/solix/blob/main/LICENSE) file for more details.

## 👨‍💻 Author

Developed by **Vishal Ahirwar**  
[GitHub Profile](https://github.com/vishal-ahirwar)

---

<<<<<<< HEAD
## Usage

Run Solix with the following syntax:
```bash
Solix [command] projectname
```

### Available Commands

| Command         | Description                                                                                         |
|------------------|-----------------------------------------------------------------------------------------------------|
| **create**       | Create a new C/C++ project. Example: `Solix create myProject`                                       |
| **subproject**   | Create Subproject<Executable/Library> C/C++ project. Example: `Solix subproject network`                                       |
| **compile**      | Compile the project. Example: `cd myProject && Solix compile -DCMAKE_BUILD_TYPE=Debug`              |
| **run**          | Run the compiled binary. Ensure the project is compiled first.                                    |
| **build**        | Compile and run the project in a single step.                                                     |
| **setup**        | Install `clang`, `vcpkg`, `Ninja` and `cmake` on your system (if not already installed).                              |
| **tests**        | Enable unit testing. Example: `Solix  tests `                                           |
| **createinstaller** | Create a packaged build of your application.                                                   |
| **update**       | Update Solix to the latest version.                                                                |
| **add**          | Add an external library using VCPKG. Example: `Solix add fmt/11.0.2`                               |
| **reload**       | Reload the package list. Example: `Solix reload`.                                                 |
| **initVCPKG**    | Add a VCPKG configuration file to the project.                                                   |
| **debug**        | Start the GDB debugger for your project.                                                          |
| **release**      | Compile the application in release mode. Example: `Solix compile -DCMAKE_BUILD_TYPE=Release`.      |
| **fix**          | Fix Solix installation issues.                                                                     |

---

## Author

Developed by **Vishal Ahirwar**.  
Copyright © 2024 Vishal Ahirwar and all Contributors. All rights reserved.

---

## License

This project is licensed under the MIT License. See the [LICENSE](License.txt) file for details.

---
=======
Feel free to explore, contribute, and start building faster with **Solix**!
>>>>>>> e857387e537ffb402c47b1a421c63fd413504949
