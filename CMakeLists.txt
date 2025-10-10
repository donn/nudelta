cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0110 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(nudelta)

# Get version from package.json
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/package.json PACKAGE_JSON)
string(JSON NUDELTA_VERSION GET ${PACKAGE_JSON} "version")
set(PROJECT_VERSION ${NUDELTA_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_SHARED_LIBS FALSE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.3")

if(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

# Dependencies
include(third_party/CMakeLists.txt)

# Common
include_directories(include)

# YAML Data
file (GLOB_RECURSE yml_files "res/**/*.yml")
find_program(RUBY_EXECUTABLE ruby)
if(RUBY_EXECUTABLE)
    message(STATUS "ruby found at: ${RUBY_EXECUTABLE}")
else()
    message(FATAL_ERROR "ruby not found in PATH and is required for an intermediate build step.")
endif()
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
    COMMAND ${RUBY_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/util/res2cxx.rb ${CMAKE_CURRENT_LIST_DIR}/res > ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
    DEPENDS ${yml_files} util/res2cxx.rb
)
add_custom_target(res_file ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)

include_directories(res)

# libnd
include_directories(${CMAKE_JS_INC})
file(GLOB nudelta_lib_src "lib/*.cpp")
add_library(nd ${nudelta_lib_src} ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)
add_dependencies(nd res_file)
target_link_libraries(nd PUBLIC fmt)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(nd PUBLIC hidapi::hidraw)
else()
        target_link_libraries(nd PUBLIC hidapi)
endif()
target_link_libraries(nd PRIVATE yaml-cpp)
target_link_libraries(nd PRIVATE $<COMPILE_ONLY:scope_guard>)

if(!MSVC)
  target_compile_options(nd -Wall -Wextra -Wpedantic -Werror)
endif()

if(DEFINED NODE_RUNTIME)
    # node-libnd
    add_definitions(-DNAPI_VERSION=4)
    include_directories(${CMAKE_JS_INC})
    include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)
    include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-api-headers/include)

    add_library(node-libnd SHARED src/node.cpp ${CMAKE_JS_SRC})
    target_link_libraries(node-libnd nd)
    target_link_libraries(node-libnd ${CMAKE_JS_LIB})

    set_target_properties(node-libnd PROPERTIES PREFIX "" SUFFIX ".node")
endif()

# nudelta
add_executable(nudelta-cli src/main.cpp)
add_compile_definitions(NUDELTA_VERSION="${CMAKE_PROJECT_VERSION}")
target_link_libraries(nudelta-cli PUBLIC nd)
target_link_libraries(nudelta-cli PRIVATE $<COMPILE_ONLY:scope_guard>)
target_link_libraries(nudelta-cli PRIVATE ssco)



install(TARGETS nudelta-cli)
