# Task2Track

## 在 Linux / WSL 中构建

需要 C++20 编译器、Make、CMake 3.21 或更新版本，以及vcpkg
依赖已在 `vcpkg.json` 中声明，配置时由 vcpkg 安装
如果 vcpkg 安装在其他位置，请修改 `CMakePresets.json` 的 `toolchainFile`

在项目根目录执行：

```bash
cmake --preset linux-vcpkg
cmake --build --preset linux-vcpkg
```

生成的程序为 `bin/main`。

使用 VS Code 时，请通过 WSL 扩展在 Linux 环境中打开项目，执行
`CMake: Select Configure Preset`，选择 `linux-vcpkg`，然后执行
`CMake: Configure` 和 `CMake: Build`。

## 找不到 cxxopts 的原因和处理方法

`find_package(cxxopts CONFIG REQUIRED)` 需要通过 vcpkg 工具链找到依赖。
仅有 `vcpkg.json` 并不会自动启用 vcpkg。工具链必须在构建目录首次配置、
执行 `project()` 时加载；在旧缓存中补上 `CMAKE_TOOLCHAIN_FILE` 可能仍然无效。

项目预设在首次配置时指定工具链，并统一使用 `build-vcpkg` 构建目录。
若已有缓存未正确加载工具链，使用 CMake 3.24 或更新版本重新配置：

```bash
cmake --fresh --preset linux-vcpkg
cmake --build --preset linux-vcpkg
```

也可以在不使用预设时显式指定工具链（以下 `--fresh` 同样需要 CMake 3.24+）：

```bash
cmake --fresh -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```
