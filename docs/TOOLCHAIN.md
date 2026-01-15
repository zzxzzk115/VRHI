# VRHI Toolchain and Development Workflow

## Overview

VRHI uses a comprehensive toolchain to ensure code quality, consistency, and cross-platform compatibility. This document covers the development tools, code standards, and CI/CD workflows.

## Development Tools

### Required Tools

- **CMake 3.21+**: Build system generator
- **C++23 Compiler**: GCC 13+, Clang 17+, MSVC 2022+
- **Ninja**: Recommended build tool (faster than Make)

### Code Quality Tools

#### clang-format

Automatic code formatting to maintain consistent style across the codebase.

**Configuration**: `.clang-format`
**Based on**: LLVM style with C++23 customizations

**Usage**:
```bash
# Format all source files
find include src tests examples -name '*.hpp' -o -name '*.cpp' | xargs clang-format -i

# Check formatting without modifying files
clang-format --dry-run --Werror <file>
```

**Key Style Rules**:
- **Indentation**: 4 spaces (no tabs)
- **Line length**: 120 characters max
- **Braces**: Attach style (K&R)
- **Pointers/References**: Left-aligned (`int* ptr`, `int& ref`)
- **Include order**: Project headers → C++ STL → C STL → Third-party → System

#### clang-tidy

Static analysis tool for finding bugs, code smells, and enforcing best practices.

**Configuration**: `.clang-tidy`

**Enabled Check Categories**:
- `bugprone-*`: Potential bugs and logic errors
- `cert-*`: CERT secure coding guidelines
- `clang-analyzer-*`: Clang static analyzer checks
- `cppcoreguidelines-*`: C++ Core Guidelines
- `modernize-*`: Modern C++ idioms (C++23)
- `performance-*`: Performance optimizations
- `readability-*`: Code readability improvements

**Usage**:
```bash
# Run clang-tidy on all sources
find include src -name '*.cpp' -o -name '*.hpp' | xargs clang-tidy -p build

# Run on specific file
clang-tidy -p build src/Core/Device.cpp
```

#### clangd

Language Server Protocol (LSP) implementation for C++, providing IDE features.

**Features**:
- Code completion
- Go-to-definition
- Find references
- Inline diagnostics
- Code actions

**Setup**:

Create `.clangd` configuration file (optional, uses `.clang-tidy` by default):
```yaml
CompileFlags:
  Add: [-std=c++23, -Wall, -Wextra]
  Remove: [-W*]
Diagnostics:
  ClangTidy:
    Add: [performance-*, modernize-*]
    Remove: [readability-identifier-length]
```

**VSCode**: Install "clangd" extension
**Neovim**: Use `nvim-lspconfig` with clangd
**CLion/QtCreator**: Built-in support

### compile_commands.json

CMake automatically generates `compile_commands.json` when configured with:
```bash
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

This file is used by:
- **clangd**: For accurate code analysis
- **clang-tidy**: For understanding compilation context
- **IDEs**: For precise symbol resolution

**Location**: `build/compile_commands.json`

**Symlinking to project root** (recommended for IDEs):
```bash
ln -s build/compile_commands.json compile_commands.json
```

## Code Standards

### Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Namespace | CamelCase | `VRHI::Core` |
| Class/Struct | CamelCase | `Device`, `BufferDesc` |
| Function/Method | CamelCase | `CreateDevice()`, `GetSize()` |
| Variable | camelBack | `deviceHandle`, `bufferSize` |
| Parameter | camelBack | `deviceConfig`, `maxCount` |
| Private Member | `m_` prefix | `m_deviceHandle`, `m_bufferPool` |
| Constant | UPPER_CASE | `MAX_FRAMES_IN_FLIGHT` |
| Enum Value | CamelCase | `BackendType::Vulkan` |

### File Organization

**Headers** (`.hpp`):
- Public API headers in `include/VRHI/`
- Implementation headers in `src/<Subsystem>/`
- One class per header file (generally)

**Sources** (`.cpp`):
- Implementation files in `src/<Subsystem>/`
- Match corresponding header name

**Example Structure**:
```
include/VRHI/
  Device.hpp          # Public API
src/Core/
  Device.cpp          # Implementation
  DeviceImpl.hpp      # Private implementation details
```

### Include Guards

Use `#pragma once` (supported by all modern compilers):
```cpp
#pragma once

namespace VRHI {
// ...
}
```

### Documentation

Use Doxygen-style comments for public APIs:
```cpp
/**
 * @brief Creates a graphics device with the specified configuration.
 * 
 * @param config Device configuration parameters
 * @return Expected containing unique_ptr to Device on success, or Error on failure
 */
std::expected<std::unique_ptr<Device>, Error> CreateDevice(const DeviceConfig& config = {});
```

## CI/CD Workflows

### GitHub Actions Configuration

**File**: `.github/workflows/ci.yml`

### CI Jobs

#### 1. Format Check
- **Platform**: Ubuntu Latest
- **Tool**: clang-format-17
- **Purpose**: Ensure all code follows formatting standards
- **Failure**: Any formatting violation fails the build

#### 2. Static Analysis
- **Platform**: Ubuntu Latest
- **Tool**: clang-tidy-17
- **Purpose**: Detect potential bugs and enforce best practices
- **Runs on**: All source files in `include/` and `src/`

#### 3. Multi-Platform Build Matrix

**Linux** (ubuntu-latest):
- **Compilers**: GCC 13, Clang 17
- **Architectures**: x64, ARM64 (cross-compile)
- **Backends**: Vulkan, OpenGL
- **Tests**: Enabled on x64

**Windows** (windows-latest):
- **Compilers**: MSVC 2022, Clang-CL
- **Architectures**: x64, ARM64
- **Backends**: Vulkan, OpenGL, D3D12 (future)
- **Tests**: Enabled on x64

**macOS** (macos-latest):
- **Compiler**: Apple Clang
- **Architectures**: x64 (Intel), ARM64 (Apple Silicon)
- **Backends**: OpenGL 4.1, Vulkan (via MoltenVK)
- **Tests**: Enabled on both architectures

#### 4. Code Coverage
- **Platform**: Linux (Ubuntu)
- **Tool**: lcov + codecov
- **Coverage Target**: >80%
- **Excluded**: External dependencies, test code

### Build Verification Matrix

Total CI configurations: **12 build jobs**
- Linux: GCC x64, GCC ARM64, Clang x64, Clang ARM64 = 4 jobs
- Windows: MSVC x64, MSVC ARM64, Clang-CL x64, Clang-CL ARM64 = 4 jobs
- macOS: x64, ARM64 = 2 jobs
- Static analysis: 1 job
- Format check: 1 job

**Total runtime**: ~45-60 minutes (parallel execution)

## Local Development Workflow

### Initial Setup

1. **Clone repository**:
```bash
git clone https://github.com/zzxzzk115/VRHI.git
cd VRHI
```

2. **Install dependencies**:
```bash
# Linux (Ubuntu/Debian)
sudo apt-get install cmake ninja-build clang-17 clang-format-17 clang-tidy-17

# macOS
brew install cmake ninja llvm

# Windows
# Install Visual Studio 2022 with C++ workload
# Install CMake and Ninja via installer or package manager
```

3. **Configure build**:
```bash
cmake -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DVRHI_BACKEND_VULKAN=ON \
  -DVRHI_BACKEND_OPENGL=ON \
  -DVRHI_BUILD_TESTS=ON \
  -DVRHI_BUILD_EXAMPLES=ON
```

4. **Symlink compile_commands.json** (for clangd):
```bash
ln -s build/compile_commands.json compile_commands.json
```

### Development Cycle

1. **Write code** using your IDE with clangd support
2. **Format code** before committing:
```bash
git diff --name-only --cached | grep -E '\.(cpp|hpp)$' | xargs clang-format -i
```

3. **Run static analysis**:
```bash
clang-tidy -p build src/Core/NewFeature.cpp
```

4. **Build and test**:
```bash
cmake --build build
cd build && ctest --output-on-failure
```

5. **Commit** following conventional commits:
```bash
git commit -m "feat: Add new feature X"
```

### Pre-Commit Hook (Recommended)

Create `.git/hooks/pre-commit`:
```bash
#!/bin/bash
# Format staged files
git diff --name-only --cached | grep -E '\.(cpp|hpp)$' | xargs -r clang-format -i

# Re-add formatted files
git diff --name-only --cached | grep -E '\.(cpp|hpp)$' | xargs -r git add

# Run quick static analysis on changed files
git diff --name-only --cached | grep -E '\.(cpp|hpp)$' | xargs -r clang-tidy -p build
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

## IDE Integration

### Visual Studio Code

**Recommended Extensions**:
- `llvm-vs-code-extensions.vscode-clangd` - clangd LSP
- `ms-vscode.cmake-tools` - CMake integration
- `twxs.cmake` - CMake syntax highlighting

**Settings** (`.vscode/settings.json`):
```json
{
  "clangd.arguments": [
    "--compile-commands-dir=build",
    "--background-index",
    "--clang-tidy"
  ],
  "cmake.configureArgs": [
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
  ]
}
```

### CLion

- Enable "Use compile_commands.json" in Settings → Build, Execution, Deployment → CMake
- Enable Clang-Tidy inspections in Settings → Editor → Inspections

### Neovim

Example `nvim-lspconfig` setup:
```lua
require('lspconfig').clangd.setup{
  cmd = {
    "clangd",
    "--compile-commands-dir=build",
    "--background-index",
    "--clang-tidy"
  }
}
```

## Troubleshooting

### clangd can't find headers
- Ensure `build/compile_commands.json` exists
- Symlink or configure clangd to use `--compile-commands-dir=build`
- Reconfigure CMake with `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`

### clang-tidy errors
- Check that compilation succeeds first
- Verify `compile_commands.json` is up to date
- Run with `-p build` to specify compilation database

### Format check fails in CI
- Run `clang-format -i` on all sources locally
- Ensure clang-format version matches CI (version 17)
- Check for tabs vs. spaces issues

## Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [clangd Documentation](https://clangd.llvm.org/)
- [clang-format Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [clang-tidy Checks](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
