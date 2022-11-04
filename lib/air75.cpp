/*
    Nudelta Console
    Copyright (C) 2022 Mohamed Gaber

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "air75.hpp"

#include "access.hpp"

#include <algorithm>
#include <scope_guard.hpp>
#include <yaml-cpp/yaml.h>

hid_device *Air75::handle() {
    return hid_open_path(path.c_str());
}

std::optional< Air75 > Air75::find() {
    std::optional< Air75 > keyboard;

    auto seeker = hid_enumerate(0x05ac, 0x024f);
    SCOPE_EXIT {
        hid_free_enumeration(seeker);
    };

    bool multipleWarned = false;
    while (seeker != nullptr) {
        // Check that the manufacturer string isn't Apple or something, avoid
        // clashing with other keyboards
        //
        // manufacturer_string appears to be null on Linux, so we can't test
        // much there :)
        bool manufacturerStringOK = true;
        if (seeker->manufacturer_string != nullptr
            && to_utf8(seeker->manufacturer_string) != std::string("BY Tech")) {
            manufacturerStringOK = false;
        }
        if (seeker->interface_number == 1) {
            if (keyboard.has_value()) {
                // We only care if the path is different, because that means a
                // different device entirely
                if (keyboard.value().path != std::string(seeker->path)) {
                    if (!multipleWarned) {
                        p(stderr,
                          "[Warning] Multiple NuPhy Air75 keyboards found! Please keep only one plugged in. Only the first matched device will be used.\n"
                        );
                        multipleWarned = true;
                    }
                }
            } else {

                if (seeker->product_string == nullptr) {
                    throw permissions_error(hidAccessFailureMessage);
                }
                keyboard = Air75(
                    seeker->product_string,
                    seeker->path,
                    seeker->release_number
                );
            }
        }
        seeker = seeker->next;
    }

    if (keyboard.has_value()) {
        return keyboard;
    }

    return std::nullopt;
}

static const size_t MAX_READABLE_SIZE = 0x7FF;
static const size_t REQUEST_SIZE = 6;
static const uint8_t REQUEST_0[] = {0x05, 0x83, 0xb6, 0x00, 0x00, 0x00};
static const uint8_t REQUEST_1[] = {0x05, 0x88, 0xb8, 0x00, 0x00, 0x00};
static const uint8_t REQUEST_2[] =
    {0x05, 0x84, 0xd8, 0x00, 0x00, 0x00}; // Keymap

static int get_report(
    hid_device *handle,
    const uint8_t *requestInfo,
    uint8_t *readBuffer
) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        throw std::runtime_error(hidAccessFailureMessage);
    }
    
    auto bytesWritten =
        hid_send_feature_report(handle, requestInfo, REQUEST_SIZE);
    if (bytesWritten < 0) {
        throw std::runtime_error("Failed to write to keyboard");
    } else {
        d("Wrote {} bytes.\n", bytesWritten);
    }
    readBuffer[0] = 0x06;
    auto bytesRead =
        hid_get_feature_report(handle, readBuffer, MAX_READABLE_SIZE);
    if (bytesRead < 0) {
        throw std::runtime_error("Failed to read from keyboard");
    } else {
        d("Read {} bytes.\n", bytesRead);
    }

    return bytesRead;
}

static std::vector< uint8_t >
get_report(hid_device *handle, const uint8_t *requestInfo) {
    uint8_t readBuffer[2048];

    auto read = get_report(handle, requestInfo, readBuffer);

    return std::vector< uint8_t >(readBuffer, readBuffer + read);
}

static const uint8_t KEYMAP_WRITE_HEADER[] =
    {0x06, 0x04, 0xd8, 0x00, 0x40, 0x00, 0x00, 0x00};

void set_report(hid_device *handle, uint8_t *data, size_t dataSize) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        throw std::runtime_error(hidAccessFailureMessage);
    }
    auto bytesWritten = hid_send_feature_report(handle, data, dataSize);
    if (bytesWritten < 0) {
        throw std::runtime_error("Failed to write to keyboard.");
    } else {
        d("Wrote {} bytes.\n", bytesWritten);
    }
}

std::vector< uint8_t > Air75::request0() {
    auto current = handle();
    SCOPE_EXIT {
        hid_close(current);
    };

    return get_report(current, REQUEST_0);
}

std::vector< uint8_t > Air75::request1() {
    auto current = handle();
    SCOPE_EXIT {
        hid_close(current);
    };

    return get_report(current, REQUEST_1);
}

std::vector< uint32_t > Air75::getKeymap() {
    auto current = handle();
    SCOPE_EXIT {
        hid_close(current);
    };

    auto keymapReport = get_report(current, REQUEST_2);

    // ALERT: Endianness-defined Behavior
    auto *start_pointer = (uint32_t *)&keymapReport[8];
    auto *end_pointer = (uint32_t *)&(*keymapReport.end());

    return std::vector< uint32_t >(start_pointer, end_pointer);
}

void Air75::setKeymap(const std::vector< uint32_t > &keymap) {
    auto current = handle();
    SCOPE_EXIT {
        hid_close(current);
    };

    size_t count = (sizeof KEYMAP_WRITE_HEADER) + (keymap.size() * 4);

    uint8_t *buffer = new uint8_t[count];
    SCOPE_EXIT {
        delete[] buffer;
    };

    // ALERT: Endianness-defined Behavior
    auto *start_pointer = (uint8_t *)&(*keymap.begin());
    auto *end_pointer = (uint8_t *)&(*keymap.end());

    std::copy(
        KEYMAP_WRITE_HEADER,
        KEYMAP_WRITE_HEADER + sizeof KEYMAP_WRITE_HEADER,
        buffer
    );

    std::copy(start_pointer, end_pointer, buffer + sizeof KEYMAP_WRITE_HEADER);
    set_report(current, buffer, count);
}

void Air75::validateYAMLKeymap(const std::string &yamlString, bool rawOk) {
    auto config = YAML::Load(yamlString);

    auto keys = config["keys"];
    if (keys.Type() != YAML::NodeType::Map) {
        throw std::runtime_error(
            "Invalid config file: key 'keys' is not a map.\n"
        );
    }

    for (auto entry : keys) {
        auto keyID = entry.first.as< std::string >();

        auto keyIt = Air75::indicesByKeyName.find(keyID);
        if (Air75::indicesByKeyName.find(keyID)
            == Air75::indicesByKeyName.end()) {
            auto errorMessage = fmt::format(
                "Invalid config file: a key for '{}' does not exist.\n",
                keyID
            );
            throw std::runtime_error(errorMessage);
        }

        auto key = keyIt->second;

        auto codeObject = entry.second;
        if (entry.second.IsScalar()) {
            auto codeID = codeObject.as< std::string >();
            codeObject = YAML::Node();
            codeObject["key"] = codeID;
        }

        // If "raw" exists: just set it and ignore everything else
        auto raw = codeObject["raw"];
        if (raw.IsDefined() && !raw.IsNull()) {
            if (!rawOk) {
                auto errorMessage = fmt::format(
                    "Raw configurations are not supported by the Nudelta GUI.\n",
                    keyID
                );
                throw std::runtime_error(errorMessage);
            }
            continue;
        }

        auto codeID = codeObject["key"].as< std::string >();
        auto codeIt = Air75::keycodesByKeyName.find(codeID);
        if (Air75::keycodesByKeyName.find(codeID)
            == Air75::keycodesByKeyName.end()) {
            auto errorMessage = fmt::format(
                "Invalid config file: a code for key '{}' was not found.",
                codeID
            );
            throw std::runtime_error(errorMessage);
        }

        auto code = codeIt->second;
        auto modifiers = codeObject["modifiers"];
        if (modifiers.IsDefined() and !modifiers.IsNull()) {
            if (modifiers.Type() != YAML::NodeType::Sequence) {
                throw std::runtime_error(fmt::format(
                    "Invalid config file: {}.modifiers is not an array.",
                    keyID
                ));
            }
            for (auto modifier : modifiers) {
                auto modifierName = modifier.as< std::string >();
                auto modifierIt =
                    Air75::modifiersByModifierName.find(modifierName);
                if (modifierIt == Air75::modifiersByModifierName.end()) {
                    throw std::runtime_error(fmt::format(
                        "Unknown modifier {}: make sure you're not adding a direction, e.g. lalt instead of alt",
                        modifierName
                    ));
                }
            }
        }
    }
}

void Air75::setKeymapFromYAML(const std::string &yamlString) {
    validateYAMLKeymap(yamlString);

    auto config = YAML::Load(yamlString);

    auto writableKeymap = Air75::defaultKeymap;
    auto keys = config["keys"];
    for (auto entry : keys) {
        auto keyID = entry.first.as< std::string >();

        auto keyIt = Air75::indicesByKeyName.find(keyID);
        auto key = keyIt->second;

        auto codeObject = entry.second;
        if (entry.second.IsScalar()) {
            auto codeID = codeObject.as< std::string >();
            codeObject = YAML::Node();
            codeObject["key"] = codeID;
        }

        // If "raw" exists: just set it and ignore everything else
        auto raw = codeObject["raw"];
        if (raw.IsDefined() && !raw.IsNull()) {
            writableKeymap[key] = raw.as< uint32_t >();
            continue;
        }

        auto codeID = codeObject["key"].as< std::string >();
        auto codeIt = Air75::keycodesByKeyName.find(codeID);

        auto code = codeIt->second;
        auto modifiers = codeObject["modifiers"];
        if (modifiers.IsDefined() and !modifiers.IsNull()) {
            for (auto modifier : modifiers) {
                auto modifierName = modifier.as< std::string >();
                auto modifierIt =
                    Air75::modifiersByModifierName.find(modifierName);
                auto modifierCode = modifierIt->second;
                code |= modifierCode;
            }
        }
        writableKeymap[key] = code;
    }
    setKeymap(writableKeymap);
}

void Air75::resetKeymap() {
    setKeymap(Air75::defaultKeymap);
}