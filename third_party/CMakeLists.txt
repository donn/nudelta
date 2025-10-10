include(FetchContent)

## scope-guard
FetchContent_Declare(
  scope-guard
  GIT_REPOSITORY https://github.com/Neargye/scope_guard
  GIT_REV 05494fbbc3ec01a93b31c2b1c830b552f5ba08f4
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(scope-guard)

## ssco
FetchContent_Declare(
  ssco
  GIT_REPOSITORY https://github.com/donn/ssco
  GIT_REV 6ebbea4de2f3f47180db8b92f19fc419f2de1d7d
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(ssco)

## hidapi
FetchContent_Declare(
  hidapi
  GIT_REPOSITORY https://github.com/libusb/hidapi
  GIT_REV 9904cbea94b9f7268a4bb526a1d32ac36e699aee
  GIT_SHALLOW ON
)

### Hidapi does not use `option`
set(HIDAPI_WITH_LIBUSB FALSE)
set(HIDAPI_WITH_HIDRAW TRUE)

FetchContent_MakeAvailable(hidapi)

## yaml-cpp
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
  GIT_SHALLOW ON
)

option(YAML_CPP_BUILD_TOOLS OFF)
FetchContent_MakeAvailable(yaml-cpp)

## fmt
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.0.2
  GIT_SHALLOW ON
)

# Force fmtlib to always be a static lib if we pull it via FetchContent (i.e.
# not intercepted by find_package), since it's a private dependency that's
# mostly header-only already.
set(saved_build_shared_libs ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(fmt)
set(BUILD_SHARED_LIBS ${saved_build_shared_libs})
