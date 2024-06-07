# CMakeList.txt: Sparrow 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.20)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (MSVC)
  add_compile_options(/bigobj)
else()
  add_compile_options(-bigobj)
endif()

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
set(CMAKE_BUILD_PARALLEL_LEVEL 32)

project ("Sparrow" VERSION 1.0)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
elseif(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

file(COPY ${CMAKE_SOURCE_DIR}/conf DESTINATION ${CMAKE_BINARY_DIR})

find_package(Boost CONFIG REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${OpenSSL_LIBRARY_DIRS})

add_library(common_libraries INTERFACE)

target_link_libraries(common_libraries INTERFACE
    redis++::redis++ 
    OpenSSL::SSL  
    protobuf::libprotobuf 
    spdlog::spdlog
)

if (WIN32)
    target_link_libraries(common_libraries INTERFACE
        pdh.lib
    )
elseif(UNIX AND NOT APPLE)
    #target_link_options(common_libraries INTERFACE)
endif()

add_subdirectory (auth_server)
add_subdirectory (route_server)
add_subdirectory (business_server)

set_property(TARGET auth_server PROPERTY CXX_STANDARD 23)
set_property(TARGET route_server PROPERTY CXX_STANDARD 23)
set_property(TARGET business_server PROPERTY CXX_STANDARD 23)
