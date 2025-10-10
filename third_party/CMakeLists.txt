include(FetchContent)

## yaml-cpp
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
  GIT_SHALLOW ON
)

find_package(yaml-cpp QUIET)
if (NOT yaml-cpp_FOUND)
  option(YAML_CPP_BUILD_TOOLS OFF)
  FetchContent_MakeAvailable(yaml-cpp)
endif()

## scope_guard
FetchContent_Declare(
  scope_guard
  GIT_REPOSITORY https://github.com/Neargye/scope_guard
  GIT_REV 05494fbbc3ec01a93b31c2b1c830b552f5ba08f4
  GIT_SHALLOW ON
)
find_package(scope_guard QUIET)
if (NOT scope_guard_FOUND)
  FetchContent_MakeAvailable(scope_guard)
endif()

## ssco
FetchContent_Declare(
  ssco
  GIT_REPOSITORY https://github.com/donn/ssco
  GIT_REV 6ebbea4de2f3f47180db8b92f19fc419f2de1d7d
  GIT_SHALLOW ON
)
find_package(ssco QUIET)
if (NOT ssco_FOUND)
  FetchContent_MakeAvailable(ssco)
endif()

## hidapi
FetchContent_Declare(
  hidapi
  GIT_REPOSITORY https://github.com/libusb/hidapi
  GIT_REV 9904cbea94b9f7268a4bb526a1d32ac36e699aee
  GIT_SHALLOW ON
)

find_package(hidapi QUIET)
if (NOT hidapi_FOUND)
  ### Hidapi does not use `option`
  set(HIDAPI_WITH_LIBUSB FALSE)
  set(HIDAPI_WITH_HIDRAW TRUE)
  FetchContent_MakeAvailable(hidapi)
endif()

## fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.0.2
  GIT_SHALLOW ON
)

find_package(fmt QUIET)
if (NOT fmt_FOUND)
  set(BUILD_SHARED_LIBS OFF)
  FetchContent_MakeAvailable(fmt)
endif()
