#ifndef _air75_hpp
#define _air75_hpp
#include "defer.hpp"

#include <codecvt>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <exception>
#include <fmt/core.h>
#include <hidapi/hidapi.h>
#include <locale>
#include <optional>
#include <string>
#include <vector>

std::string to_utf8(std::wstring in) {
    std::wstring_convert< std::codecvt_utf8< wchar_t > > myconv;
    return myconv.to_bytes(in);
}

class Air75 {
    public:
        uint16_t firmware;
        std::string productString;
        std::string path;

        Air75(std::wstring productString, std::string path, uint16_t firmware)
            : productString(to_utf8(productString)), path(path),
              firmware(firmware) {}

        hid_device *handle() { return hid_open_path(path.c_str()); }
};

std::optional< Air75 > get_air75() {
    std::optional< Air75 > keyboard;

    auto seeker = hid_enumerate(0x05ac, 0x024f);
    defer {
        hid_free_enumeration(seeker);
    };

    bool multipleWarned = false;
    while (seeker != nullptr) {
        if (seeker->interface_number == 1) {
            if (keyboard.has_value()) {
                if (!multipleWarned) {
                    fmt::print(
                        stderr,
                        "[Warning] Multiple NuPhy Air75 keyboards found! Please keep only one plugged in. Only the first matched device will be used.\n");
                    multipleWarned = true;
                }
            } else {

                if (seeker->product_string == nullptr) {
                    throw std::runtime_error(
                        "Couldn't read HID product string");
                }
                keyboard = Air75(
                    seeker->product_string,
                    seeker->path,
                    seeker->release_number);
            }
        }
        seeker = seeker->next;
    }

    if (keyboard.has_value()) {
        return keyboard;
    }

    return std::nullopt;
}
#endif