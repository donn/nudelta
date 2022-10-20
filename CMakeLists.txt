
cmake_policy(SET CMP0110 NEW)
cmake_policy(SET CMP0048 NEW)

project(nudelta VERSION 0.1.1 LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package (Python3 COMPONENTS Interpreter)
set(HIDAPI_WITH_LIBUSB TRUE)
set(HIDAPI_WITH_HIDRAW FALSE)
set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(submodules/hidapi)
add_subdirectory(submodules/fmt)
add_subdirectory(submodules/yaml-cpp)

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

# YAML Data
add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
     COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/res/to_cpp.py > ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
)
add_custom_target(res_file ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)


# Nudelta Binary
file(GLOB nudelta_src "src/*.cpp")
include_directories(include)
include_directories(res)

add_executable(nudelta ${nudelta_src} ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)
add_dependencies(nudelta res_file)
add_compile_definitions(NUDELTA_VERSION="${CMAKE_PROJECT_VERSION}")
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(nudelta PRIVATE hidapi::libusb)
else ()
	target_link_libraries(nudelta PRIVATE hidapi)
endif ()
target_link_libraries(nudelta PRIVATE yaml-cpp)
target_link_libraries(nudelta PRIVATE fmt)

install(TARGETS nudelta)