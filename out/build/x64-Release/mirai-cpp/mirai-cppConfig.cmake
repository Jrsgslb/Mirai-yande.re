include(CMakeFindDependencyMacro)

# 如果想要获取Config阶段的变量，可以使用这个
# set(my-config-var )

# 项目依赖项

# Any extra setup

# Add the targets file
include("${CMAKE_CURRENT_LIST_DIR}/mirai-cppTargets.cmake")
