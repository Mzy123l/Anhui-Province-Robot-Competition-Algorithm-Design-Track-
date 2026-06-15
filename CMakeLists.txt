# CMakeList.txt: Tasks 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.10)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project ("Tasks")

file(GLOB SOURCES "src/*.cpp")
# 将源代码添加到此项目的可执行文件。
add_executable (Tasks "Tasks.cpp" ${SOURCES} )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Tasks PROPERTY CXX_STANDARD 23)
endif()
if(MSVC)
    target_link_options(Tasks PRIVATE /STACK:16777216)   # 16 MB
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Linux 下常用 --stack，macOS 需用 -Wl,-stack_size,...
    if(APPLE)
        target_link_options(Tasks PRIVATE -Wl,-stack_size,0x1000000)   # 16 MB
    else()
        target_link_options(Tasks PRIVATE -Wl,--stack,16777216)        # 16 MB
    endif()
endif()
# TODO: 如有需要，请添加测试并安装目标。
