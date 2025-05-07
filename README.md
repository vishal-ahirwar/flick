
# Solix - A Modern C++ Project Setup & Build System

Solix is a fast, reliable, and cross-platform build system for C++ projects. It automates common tasks, like integrating external libraries, managing CMake configuration, and providing an efficient build system to save time for developers.

## 🚀 Why Use Solix?

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

The only way to build **Solix** from source is with CMake. Follow these simple steps:

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
   git clone https://github.com/vishal-ahirwar/solix.git
   cd solix
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

Feel free to explore, contribute, and start building faster with **Solix**!
