![Logo](logo.svg)

# ProjectBones - C++ starter pack

![C++](https://img.shields.io/badge/C++-23-orange.svg)

## 📖 Overview

ProjectBones is a lightweight C++starter template configured with xmake, designed to accelerate the setup and bootstrap process for new C++ projects.

## 🚀 Features

- **xmake Ready:** Pre-configured `xmake.lua` for fast and clean builds out of the box.
- **Clean Project Structure:** Well-organized source and header directories for quick bootstrapping.
- **Modern C++ Setup:** Configured with modern C++ standards and best practices.
- **Lightweight Starter:** Minimal footprint, giving you total freedom to build your project.

## 📦 Installation

### Using xmake

```bash
# Install **xmake** and a **C++ toolchain** (MSVC recommended on Windows). If you encounter xrepo compatibility issues, install **Conan 2** (see Best Practices).

# Build project and documentation
xmake

# Run the application
xmake run

# Clean build
xmake clean
```

## [🎥 ](https://apps.timwhitlock.info/emoji/tables/unicode#emoji-modal)Tiny example

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!\n";
    return 0;
}
```
