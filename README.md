# 🛠️ CCC - C/C++ Project Starter and build Tool

> A simple, lightweight CLI tool to initialize and manage C/C++ projects with ease.<br>
> Note: It is just a hobby project, i thought of managing, building my c/c++ project easiely without cmake, make etc...

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)](https://github.com/57ajay/ccc)
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://github.com/57ajay/ccc)

---

## ✨ Features

- **🚀 Quick Project Setup** - Initialize new C/C++ projects with consistent structure
- **📁 Auto-Generated Layout** - Automatic folder organization for clean project structure
- **⚡ Fast Build System** - Simple CLI build using `gcc` or `g++`
- **🪶 Lightweight & Portable** - Minimal dependencies, runs anywhere
- **🌍 Cross-Platform** - Linux, macOS, and Windows (via WSL) support
- **🔗 Library Integration** - Easy external library linking
- **🔄 Flexible Integration** - Works with existing C/C++ projects

---

## 🚀 Quick Start

### Installation

#### 1. Clone the Repository
```bash
git clone https://github.com/57ajay/ccc.git
cd ccc
```

#### 2. Compile the CLI Tool
```bash
gcc -o ccc ccc.c
```

#### 3. Install Globally (Optional but highly Recommended)
**Linux/macOS:**
```bash
sudo mv ccc /usr/local/bin/
```

**Windows:**
Add the `ccc.exe` to your system PATH or use WSL.

---

## 📖 Usage Guide

### Creating a New Project

Initialize a new project in the current directory:

```bash
ccc init
```

**Interactive Setup:**
- Choose project type: `c` or `cpp`/`c++`
- Automatic folder structure generation

**Generated Structure:**
```
your-project/
├── 📁 bin/           # Compiled executables
├── 📁 src/           # Source files (.c/.cpp)
├── 📁 include/       # Header files (.h/.hpp)
└── 📄 .ccc_config    # Project configuration
```

### Building Your Project

From your project root directory:

```bash
ccc
```

✅ **What happens:**
- Compiles all source files in `src/`
- Links headers from `include/`
- Outputs executable to `bin/` directory
- Creates object files in `bin/obj/`

### Building with External Libraries

Link external libraries using the `-l` flag:

```bash
ccc -l "m pthread"
```

This appends `-lm -lpthread` to the compilation command.

**Common Libraries:**
```bash
# Math library
ccc -l "m"

# Threading and math
ccc -l "m pthread"

# Multiple libraries
ccc -l "ssl crypto curl"
```

---

## 🔧 Integration with Existing Projects

You can use `ccc` with your existing C/C++ projects:

### 1. Add Configuration File

Create a `.ccc_config` file in your project root:

```ini
project_type=c       # or cpp/c++
```

### 2. Ensure Directory Structure

Make sure these directories exist:
```bash
mkdir -p bin/obj src include
```

### 3. Build

```bash
ccc
```

---

## 🖥️ Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| 🐧 **Linux** | ✅ Full Support | Recommended platform |
| 🍎 **macOS** | ✅ Full Support | Works with Xcode Command Line Tools |
| 🪟 **Windows** | ⚠️ Limited Support | Use WSL or install GCC via MinGW |

### Windows Setup
1. **WSL (Recommended):** Install Windows Subsystem for Linux
2. **MinGW:** Install MinGW-w64 for native Windows support
3. **MSYS2:** Alternative Windows development environment

---

## 📝 Configuration Options

The `.ccc_config` file supports the following options:

```ini
# Project type (required)
project_type=c          # Options: c, cpp, c++

# NOTE: THESE ALL TO BE IMPLEMENTED 
compiler=gcc            # Options: gcc, g++, clang, clang++

# Additional flags (optional)
cflags=-Wall -Wextra -std=c11
cppflags=-Wall -Wextra -std=c++17
```

---

## 🤝 Contributing

I welcome contributions from the community! Here's how you can help:

### 🎯 How to Contribute

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature-amazing-feature`
3. **Commit** your changes: `git commit -m 'Add amazing feature'`
4. **Push** to the branch: `git push origin feature-amazing-feature`
5. **Open** a Pull Request

### 💡 Contribution Ideas

- [ ] **Clang Support** - Add native clang/clang++ compiler support
- [ ] **Enhanced Windows** - Improve native Windows compatibility
- [ ] **Auto PATH Setup** - Automatic system PATH configuration
- [ ] **Advanced Config** - More flexible `.ccc_config` options
- [ ] **Template System** - Pre-built project templates
- **You can implement your own idea too**

### 🐛 Bug Reports

Found a bug? Please [open an issue](https://github.com/57ajay/ccc/issues) with:
- Your operating system
- CCC version
- Steps to reproduce
- Expected vs actual behavior

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Thanks to all contributors who help make CCC better
- Inspired by the need for simple C/C++ project management
- Built with ❤️ for the C/C++ community

---

## 📞 Support

- 📖 **Documentation:** [Wiki](https://github.com/57ajay/ccc/README.md)
- 🐛 **Issues:** [GitHub Issues](https://github.com/57ajay/ccc/issues)

---

<div align="center">

**⭐ Star this repository if you find it helpful!**

[⬆️ Back to Top](#️-ccc---cc-project-starter-tool)

</div>
