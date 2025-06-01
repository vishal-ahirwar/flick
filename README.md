
> **Use external libraries in Modern C++ as easily as in other modern languages.**
> Flick brings the simplicity of dependency management from languages like Python, Rust, or JavaScript into C++—finally making C++ feel modern to work with.

---
|![image](https://github.com/user-attachments/assets/9bf99d88-423b-42b8-b6b8-cbc0bfbb62e8)|![image](https://github.com/user-attachments/assets/a0a98710-d695-45ea-9662-7e1ce1957ae8)|
|---|---|


## ✨ Features

* 📦 **Modern C++ Package Management**
  Easily add external libraries using `vcpkg`, without writing CMake by hand.

* ⚡ **Faster Builds**
  Optimized build presets with Clang and Ninja ensure rapid compilation.

* 🧱 **Cross-Platform & Cross-Compiling**
  Build the same project seamlessly across Windows and Linux, and target other platforms too.

* 📁 **Professional Project Structure**
  Automatically generates clean CMake-based layouts.

* 🔄 **Repeatable Builds**
  Get consistent output across machines with unified presets.

* 🎯 **Build From Scratch? Never Again**
  Save hours by never manually writing CMake files again.

* 🧪 **Unit Testing Integration**
  Enable and run tests with ease.

* 🛠️ **Installer Generation**
  Easily package your application for distribution.

* 🧰 **Environment Setup**
  Automatically installs and configures Clang, Ninja, and CMake.

---

## 🔧 Installation

### Prerequisites

* Git
* CMake
* Clang (on all platforms)
* Ninja (Recommended)
* vcpkg (Handled internally by Flick)

### Building Flick

```bash
cmake --preset=default
cmake --build build/default
```

> Or use Flick itself to bootstrap:

```bash
flick build
```
or
```bash
flick build --standalone
```
---

## 🚀 Usage

```bash
flick <Core Commands>
```

### Core Commands

| Command           | Description                                                         |
| ----------------- | ------------------------------------------------------------------- |
| `new`          | Create a new C/C++ project with all defaults                        |
| `subproject`      | Add a subproject (executable or library)                            |
| `build`           | Configure and compile using Clang + Ninja                           |
| `run`             | Run the compiled binary                                             |
| `compile`         | Compile with custom flags (e.g. Debug or Release)                   |
| `tests`           | Enable and manage unit tests                                        |
| `debug`           | Launch LLDB for debugging                                           |
| `release`         | Compile in release mode                                             |
| `install`         | Add external library using `vcpkg`. Example: `Flick add fmt/11.0.2` |
| `clean`           | Reload the vcpkg dependency cache                                   |
| `installer`       | Generate an installable package                                     |
| `doctor`          | Diagnose and fix Flick issues                                       |
| `update`          | Update Flick to the latest version                                  |
| `init`            | Auto install Clang, Ninja, CMake, and vcpkg                         |

---

## 📂 Example

```bash
$ flick create my-awesome-app
$ flick install --package=libpqxx --update-base-line
$ flick install --package=restclient-cpp --version=0.5.2 --update-base-line
$ flick install --package=qtbase
$ flick build
$ flick subproject client
$ flick build client --standalone
$ flick tests
$ flick release --standalone
```

---

## 🧠 Why Use Flick?

* Skip repetitive setup and config
* Learn modern C++ without being blocked by tooling
* Fast builds out of the box
* Cross-platform by default
* Just focus on building things

---

## 👨‍💻 Author

Made with 💻 by [Vishal Ahirwar](https://github.com/vishal-ahirwar)

---

## 📄 License

This project is licensed under the BSD 3-Clause License. See `LICENSE` for more info.
