# CMake 版本要求 (最低 3.1)
cmake_minimum_required (VERSION 3.1)
# 项目名称
project(Mirai-yande)
# 设置项目的 C++ 标准
set(CMAKE_CXX_STANDARD 11)
# 不要构建 mirai-cpp 自带的 examples
set(MIRAI_CPP_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
#添加vcpkg
set(-DCMAKE_TOOLCHAIN_FILE=D:/Dl/vcpkg-2020.07/scripts/buildsystems/vcpkg.cmake)
# 添加子模块 mirai-cpp
add_subdirectory(mirai-cpp)
#cpr库
find_package(cpr CONFIG REQUIRED)
#RapidJSON库
find_package(RapidJSON)
#链接boost库
find_path(BOOST_PROPERTY_TREE_INCLUDE_DIRS "boost/property_tree/detail/exception_implementation.hpp")
# 源码以 UTF-8 编码保存，使用 MSVC 编译器时需要加编译参数提供支持
if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    message(STATUS "optional: /UTF-8")   
endif(MSVC)

list(APPEND SOURCES src/logo.rc)

add_executable(${PROJECT_NAME} src/main.cpp   "include/yande.h" "include/message.h"  "src/yande.cpp" "src/message.cpp"   "include/imgsearch.h" "src/imgsearch.cpp"    "include/Timing.h" "src/Timing.cpp" "src/bilibili.cpp" "include/HttpRequest.hpp"  "include/Bilibili.h")
# 设置头文件目录
include_directories(include)
# 链接 mirai-cpp
target_link_libraries(${PROJECT_NAME} PRIVATE mirai-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr)
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${BOOST_PROPERTY_TREE_INCLUDE_DIRS})
# 使用 mirai-cpp 静态库
target_compile_definitions(${PROJECT_NAME} PRIVATE MIRAICPP_STATICLIB)
# gcc 要连接 pthread
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
