cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0110 NEW)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

# Read the JSON file.
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/package.json PACKAGE_JSON)
string(JSON NUDELTA_VERSION GET ${PACKAGE_JSON} "version")

project(nudelta VERSION ${NUDELTA_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package (Python3 COMPONENTS Interpreter)
set(HIDAPI_WITH_LIBUSB TRUE)
set(HIDAPI_WITH_HIDRAW FALSE)
set(BUILD_SHARED_LIBS FALSE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(submodules/scope_guard)
add_subdirectory(submodules/hidapi)
add_subdirectory(submodules/fmt)
add_subdirectory(submodules/yaml-cpp)
add_subdirectory(submodules/ssco)

# Python/PIP
find_package(
        Python3
        REQUIRED
        COMPONENTS Interpreter
)

execute_process(
        COMMAND ${Python3_EXECUTABLE} -m pip --version
        RESULT_VARIABLE PIP_VERSION_CHECK
        OUTPUT_QUIET
)
if (NOT ${PIP_VERSION_CHECK} EQUAL 0)
    message(
            FATAL_ERROR
            "PIP is not installed.\nYou may want to install it using the following command:\n 
            '${Python3_EXECUTABLE} -m ensurepip'"
    )
endif()

execute_process(
        COMMAND ${Python3_EXECUTABLE} -m pip show pyyaml
        RESULT_VARIABLE PYYAML_CHECK
        OUTPUT_QUIET
)
if (NOT ${PYYAML_CHECK} EQUAL 0)
    message(
            FATAL_ERROR
            "The 'pyyaml' package is not installed.\nYou may want to install it using the following command:\n  ${Python3_EXECUTABLE} -m pip install pyyaml"
    )
endif()

# Common
include_directories(include)

# YAML Data
file (GLOB yml_files "res/*.yml")
add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
     COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/res/to_cpp.py > ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
     DEPENDS ${yml_files} res/to_cpp.py
)
add_custom_target(res_file ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)

include_directories(res)

# libnd
file(GLOB nudelta_lib_src "lib/*.cpp")
add_library(nd ${nudelta_lib_src} ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)
add_dependencies(nd res_file)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(nd PRIVATE hidapi::libusb)
else ()
	target_link_libraries(nd PRIVATE hidapi)
endif ()
target_link_libraries(nd PRIVATE yaml-cpp)
target_link_libraries(nd PRIVATE fmt)
target_link_libraries(nd PRIVATE scope_guard)


# node-libnd
add_definitions(-DNAPI_VERSION=4)
include_directories(${CMAKE_JS_INC})
include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-addon-api)
include_directories(${CMAKE_SOURCE_DIR}/node_modules/node-api-headers/include)
add_library(node-libnd SHARED src/node.cpp ${CMAKE_JS_SRC})
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(node-libnd PRIVATE hidapi::libusb)
else ()
	target_link_libraries(node-libnd PRIVATE hidapi)
endif ()
target_link_libraries(node-libnd PRIVATE yaml-cpp)
target_link_libraries(node-libnd PRIVATE fmt)
target_link_libraries(node-libnd PRIVATE scope_guard)
target_link_libraries(node-libnd PRIVATE nd)

set_target_properties(node-libnd PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(node-libnd ${CMAKE_JS_LIB})

# nudelta
add_executable(nudelta src/main.cpp)
add_compile_definitions(NUDELTA_VERSION="${CMAKE_PROJECT_VERSION}")
target_link_libraries(nudelta PRIVATE nd)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(nudelta PRIVATE hidapi::libusb)
else ()
	target_link_libraries(nudelta PRIVATE hidapi)
endif ()
target_link_libraries(nudelta PRIVATE yaml-cpp)
target_link_libraries(nudelta PRIVATE fmt)
target_link_libraries(nudelta PRIVATE ssco)
target_link_libraries(nudelta PRIVATE scope_guard)


install(TARGETS nudelta)