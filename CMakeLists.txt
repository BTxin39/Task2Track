cmake_minimum_required(VERSION 3.14)
project(Task2Track CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2. 生成 compile_commands.json，供 clangd / IntelliSense 读取
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(cxxopts CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 4. 用 aux_source_directory 收集源文件（和原写法一致）
aux_source_directory(src SRC_LIST)

add_executable(main ${SRC_LIST})

# 5. 用 target_* 代替全局 include_directories，更符合现代 CMake
target_include_directories(main PRIVATE include)

# 6. 链接依赖的导入目标
target_link_libraries(main PRIVATE
    cxxopts::cxxopts
    nlohmann_json::nlohmann_json
)

# 7. 老版本 GCC 需要显式链接 stdc++fs 才能用 filesystem
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    target_link_libraries(main PRIVATE stdc++fs)
endif()