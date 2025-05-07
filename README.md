---
# Solix
[![License: BSD-3-Clause](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)
**Solix** is a powerful tool for C/C++ learners, designed to streamline the process of setting up and managing projects. With Solix, you can focus on learning and coding instead of dealing with repetitive tasks like creating folders, writing CMake files, or compiling manually.

---

## About

Learning C/C++ and tired of manually managing files, folders, and compilation? Solix automates these tasks by:

- **Automated Project Initialization**: Generate a professional-grade project structure complete with a `CMakeLists.txt` file.  
- **Organized Folder Setup**: Start every project with a clean, intuitive folder hierarchy.  
- **Default `.gitignore` Integration**: Avoid unnecessary clutter by including a pre-configured `.gitignore` file.  
- **Seamless Library Management**: Leverage built-in VCPKG support to easily integrate external libraries into your project.  
- **Quick Environment Setup**: Configure your entire C++ learning environment effortlessly with a single command.  
- **Efficiency-Driven Commands**: Simplify repetitive tasks to focus more on learning and building your C++ skills.  

No more starting from scratch every time you want to learn a new concept in C/C++! 😊

---

## Features

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

---

Download the pre-build binaries for your platform 
or
## Installation

### Prerequisites

Ensure the following tools are installed on your system:

- **Git**: [Download Git](https://git-scm.com/downloads)
- **CMake**: [Download CMake](https://cmake.org/download/)
- **G++** (Linux) or **MinGW** (Windows): 
  - Linux: Install via package manager (`sudo apt install build-essential`).
  - Windows: [Download MinGW](https://github.com/brechtsanders/winlibs_mingw/releases/).

### Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/vishal-ahirwar/Solix.git
   cd Solix
   ```

2. Build and install:
   - **Linux**:
     ```
     python3 build.py
     ```
   - **Windows**:
     ```batch
     python3 build.py
     ```

---

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
