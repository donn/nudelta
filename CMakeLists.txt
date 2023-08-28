cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0110 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(nudelta)

if(NOT DEFINED NODE_RUNTIME)
        message( FATAL_ERROR "NODE_RUNTIME is not defined. Use `cmake-js` from the root of the repo." )
endif()

# Get version from package.json
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/package.json PACKAGE_JSON)
string(JSON NUDELTA_VERSION GET ${PACKAGE_JSON} "version")
set(PROJECT_VERSION ${NUDELTA_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS FALSE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11")

if(APPLE)
        set(NODE_ARCH arm64;x86_64)
        set(CMAKE_THREAD_LIBS_INIT "-lpthread")
        set(CMAKE_HAVE_THREADS_LIBRARY 1)
        set(CMAKE_USE_WIN32_THREADS_INIT 0)
        set(CMAKE_USE_PTHREADS_INIT 1)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

# Hidapi does not use `option`
set(HIDAPI_WITH_LIBUSB FALSE)
set(HIDAPI_WITH_HIDRAW TRUE)
add_subdirectory(submodules/hidapi)

option(YAML_CPP_BUILD_TOOLS OFF)
add_subdirectory(submodules/yaml-cpp)

add_subdirectory(submodules/scope_guard)

add_subdirectory(submodules/fmt)

add_subdirectory(submodules/ssco)

# Common
include_directories(include)

# YAML Data
file (GLOB_RECURSE yml_files "res/**/*.yml")
add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
     COMMAND node ${CMAKE_CURRENT_LIST_DIR}/util/res_to_cpp.js ${CMAKE_CURRENT_LIST_DIR}/res > ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
     DEPENDS ${yml_files} util/res_to_cpp.js
)
add_custom_target(res_file ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)

include_directories(res)

# libnd
file(GLOB nudelta_lib_src "lib/*.cpp")
add_library(nd ${nudelta_lib_src} ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)
add_dependencies(nd res_file)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(nd hidapi::hidraw)
else()
        target_link_libraries(nd hidapi)
endif()
target_link_libraries(nd yaml-cpp)
target_link_libraries(nd fmt)
target_link_libraries(nd scope_guard)

if(!MSVC)
  target_compile_options(nd -Wall -Wextra -Wpedantic -Werror)
endif()


# node-libnd
add_definitions(-DNAPI_VERSION=4)
include_directories(${CMAKE_JS_INC})
include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)
include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-api-headers/include)
add_library(node-libnd SHARED src/node.cpp ${CMAKE_JS_SRC})
target_link_libraries(node-libnd nd)
target_link_libraries(node-libnd ${CMAKE_JS_LIB})

set_target_properties(node-libnd PROPERTIES PREFIX "" SUFFIX ".node")

# nudelta
add_executable(nudelta src/main.cpp)
add_compile_definitions(NUDELTA_VERSION="${CMAKE_PROJECT_VERSION}")
target_link_libraries(nudelta nd)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(nudelta hidapi::hidraw)
else()
        target_link_libraries(nudelta hidapi)
endif()
target_link_libraries(nudelta yaml-cpp)
target_link_libraries(nudelta fmt)
target_link_libraries(nudelta ssco)
target_link_libraries(nudelta scope_guard)


install(TARGETS nudelta)