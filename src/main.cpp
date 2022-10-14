#include "air75.hpp"
#include "defer.hpp"

#include <codecvt>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <exception>
#include <fmt/core.h>
#include <hidapi/hidapi.h>
#include <iostream>
#include <locale>
#include <optional>
#include <string>
#include <vector>

#define MAX_STR 255

const size_t MAX_READABLE_SIZE = 0x7FF;
const size_t REQUEST_SIZE = 6;
const uint8_t REQUEST_0[] = {0x05, 0x83, 0xb6, 0x00, 0x00, 0x00};
const uint8_t REQUEST_1[] = {0x05, 0x88, 0xb8, 0x00, 0x00, 0x00};
const uint8_t REQUEST_2[] =
    {0x05, 0x84, 0xd8, 0x00, 0x00, 0x00}; // Windows Mode Keymap

int get_report(
    hid_device *handle,
    const uint8_t *requestInfo,
    uint8_t *readBuffer) {
    auto bytesWritten =
        hid_send_feature_report(handle, requestInfo, REQUEST_SIZE);
    if (bytesWritten < 0) {
        throw std::runtime_error("Failed to write to keyboard");
    } else {
        fmt::print(stderr, "Wrote {} bytes.\n", bytesWritten);
    }
    readBuffer[0] = 0x06;
    auto bytesRead =
        hid_get_feature_report(handle, readBuffer, MAX_READABLE_SIZE);
    if (bytesRead < 0) {
        throw std::runtime_error("Failed to read from keyboard");
    } else {
        fmt::print(stderr, "Read {} bytes.\n", bytesRead);
    }

    return bytesRead;
}

int main(int argc, char *argv[]) {
    unsigned char buf[65];
    wchar_t wstr[MAX_STR];

    // Initialize the hidapi library
    if (hid_init()) {
        fmt::print(stderr, "Failed to initialize HID library.\n");
        return -1;
    }
    defer {
        hid_exit();
    };

    auto air75_optional = get_air75();
    if (!air75_optional.has_value()) {
        fmt::print(
            stderr,
            "Couldn't find a NuPhy Air75 connected to this computer.");
        return -1;
    }

    auto air75 = air75_optional.value();

    fmt::print(
        "Found NuPhy Air75 at path {} (Firmware {:04x})\n",
        air75.path,
        air75.firmware);

    auto handle = air75.handle();
    defer {
        hid_close(handle);
    };

    uint8_t readBuffer[2048];

    auto bytesRead = get_report(handle, REQUEST_0, readBuffer);
    for (size_t i = 0; i < bytesRead; i += 1) {
        std::cout << (int)readBuffer[i] << " ";
    }

    bytesRead = get_report(handle, REQUEST_1, readBuffer);
    for (size_t i = 0; i < bytesRead; i += 1) {
        std::cout << (int)readBuffer[i] << " ";
    }

    bytesRead = get_report(handle, REQUEST_2, readBuffer);
    for (size_t i = 0; i < bytesRead; i += 1) {
        std::cout << (int)readBuffer[i] << " ";
    }

    return 0;
}