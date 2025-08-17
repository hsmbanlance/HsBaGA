# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.12)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("HsBaGA" VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SAMPLES "Build samples" ON)

# 包含子项目。

if(BUILD_SAMPLES)
  add_subdirectory("Samples")
endif()

add_library(HsBaGA INTERFACE)

#安装头文件
install(DIRECTORY GA
  DESTINATION HsBa
  FILES_MATCHING PATTERN "*.hpp"
)

# 安装导出目标
install(TARGETS HsBaGA
        EXPORT HsBaGA-targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION bin
        INCLUDES DESTINATION include)

# 配置和安装 targets.cmake 文件
install(EXPORT HsBaGA-targets
        FILE HsBaGA-targets.cmake
        NAMESPACE HsBa::
        DESTINATION share/HsBaGA)

# 配置和安装 config.cmake 文件
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/config.cmake.in
    ${PROJECT_BINARY_DIR}/HsBaGA-config.cmake
    INSTALL_DESTINATION share/HsBaGA
)

# 配置和安装 version.cmake 文件
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/HsBaGA-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${PROJECT_BINARY_DIR}/HsBaGA-config.cmake
    ${PROJECT_BINARY_DIR}/HsBaGA-config-version.cmake
    DESTINATION share/HsBaGA
)