
> **Use external libraries in Modern C++ as easily as in other modern languages.**
> Flick brings the simplicity of dependency management from languages like Python, Rust, or JavaScript into C++—finally making C++ feel modern to work with.

---
|![image](https://github.com/user-attachments/assets/7711dc54-9bf3-419f-b11d-09c929871090)|![image](https://github.com/user-attachments/assets/56595300-3309-4179-b409-67e4461ecee4)|
|---|---|


---

## 🔧 Installation

[![Use Flick Installer](https://img.shields.io/badge/Use-Flick_Installer-blue?style=for-the-badge)](https://github.com/vishal-ahirwar/flick-installer)

### Or Build from Source
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
flick build --static
```
---

## 🚀 Usage

```bash
flick <Core Commands>
```

### Core Commands

| Command           | Description                                                         |
| ----------------- | ------------------------------------------------------------------- |
| `new`             | Create a new C/C++ project with all defaults                        |
| `subproject`      | Add a subproject (executable or library)                            |
| `build`           | Configure and compile using Clang + Ninja                           |
| `run`             | Run the compiled binary                                             |
| `compile`         | Compile with custom flags                   |
| `tests`           | Enable and manage unit tests                                        |
| `debug`           | Launch LLDB for debugging                                           |
| `release`         | Compile in release mode                                             |
| `install`         | Add external library                                                |
| `list`            | list down the libraries used in current project                     |
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
$ flick build client --static
$ flick tests
$ flick release --static
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
