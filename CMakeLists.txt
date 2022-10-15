project(nudelta)
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0110 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package (Python3 COMPONENTS Interpreter)
set(HIDAPI_WITH_LIBUSB TRUE)
set(HIDAPI_WITH_HIDRAW FALSE)
set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(submodules/hidapi)
add_subdirectory(submodules/fmt)
add_subdirectory(submodules/yaml-cpp)

include_directories(include)

add_custom_command(
     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
     COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/res/to_cpp.py > ${CMAKE_CURRENT_BINARY_DIR}/res.cpp
)
add_custom_target(res_file ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)

file(GLOB nudelta_src
     "src/*.cpp"
)

add_executable(nudelta ${nudelta_src} ${CMAKE_CURRENT_BINARY_DIR}/res.cpp)
add_dependencies(nudelta res_file)
target_link_options(nudelta PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(nudelta PRIVATE hidapi::libusb)
target_link_libraries(nudelta PRIVATE yaml-cpp)
target_link_libraries(nudelta PRIVATE fmt)