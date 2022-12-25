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

Air75::Handles Air75::getHandles() {
    auto dataHandle = hid_open_path(path.c_str());
    auto requestHandle = dataHandle;
    if (requestPath.has_value()) {
        requestHandle = hid_open_path(requestPath.value().c_str());
    }

    auto cleanup = [](Air75::Handles &handles) {
        if (handles.request != handles.data) {
            hid_close(handles.request);
        }
        hid_close(handles.data);
    };

    return {dataHandle, requestHandle, cleanup};
}

static const size_t MAX_READABLE_SIZE = 0x7FF;
static const size_t REQUEST_SIZE = 6;

static const uint8_t REQUEST_0[] = {0x05, 0x83, 0xb6, 0x00, 0x00, 0x00};
static const uint8_t REQUEST_1[] = {0x05, 0x88, 0xb8, 0x00, 0x00, 0x00};

static const uint8_t WINDOWS_KEYMAP_GET_HEADER[] =
    {0x05, 0x84, 0xd8, 0x00, 0x00, 0x00};
static const uint8_t MAC_KEYMAP_GET_HEADER[] =
    {0x05, 0x84, 0xd4, 0x00, 0x00, 0x00};

static int get_report(
    hid_device *dataHandle,
    hid_device *requestHandle,
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
        hid_send_feature_report(requestHandle, requestInfo, REQUEST_SIZE);
    if (bytesWritten < 0) {
        auto errorString = fmt::format(
            "Failed to write to keyboard: {}",
            to_utf8(hid_error(requestHandle))
        );
        throw std::runtime_error(errorString);
    } else {
        d("Wrote {} bytes.\n", bytesWritten);
    }
    readBuffer[0] = 0x06;
    auto bytesRead =
        hid_get_feature_report(dataHandle, readBuffer, MAX_READABLE_SIZE);
    if (bytesRead < 0) {
        auto errorString = fmt::format(
            "Failed to read from keyboard: {}",
            to_utf8(hid_error(dataHandle))
        );
        throw std::runtime_error("Failed to read from keyboard");
    } else {
        d("Read {} bytes.\n", bytesRead);
    }

    return bytesRead;
}

static std::vector< uint8_t > get_report(
    hid_device *dataHandle,
    hid_device *requestHandle,
    const uint8_t *requestInfo
) {
    uint8_t readBuffer[2048];

    auto read = get_report(dataHandle, requestHandle, requestInfo, readBuffer);

    return std::vector< uint8_t >(readBuffer, readBuffer + read);
}

static const uint8_t WINDOWS_KEYMAP_WRITE_HEADER[] =
    {0x06, 0x04, 0xd8, 0x00, 0x40, 0x00, 0x00, 0x00};
static const uint8_t MAC_KEYMAP_WRITE_HEADER[] =
    {0x06, 0x04, 0xd4, 0x00, 0x40, 0x00, 0x00, 0x00};

void set_report(hid_device *requestHandle, uint8_t *data, size_t dataSize) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        throw std::runtime_error(hidAccessFailureMessage);
    }
    auto bytesWritten = hid_send_feature_report(requestHandle, data, dataSize);
    if (bytesWritten < 0) {
        auto errorString = fmt::format(
            "Failed to write to keyboard: {}",
            to_utf8(hid_error(requestHandle))
        );
        throw std::runtime_error(errorString);
    } else {
        d("Wrote {} bytes.\n", bytesWritten);
    }
}

#ifdef _WIN32
// Win is different: there are multiple "channels" each with a different path
// - So far, I've identified col06 as the one for keymap data, col05 for
// requests
std::string writeCol = "col05";
std::string dataCol = "col06";

std::optional< Air75 > Air75::find() {
    std::optional< Air75 > keyboard;

    auto seeker = hid_enumerate(0x05ac, 0x024f);
    SCOPE_EXIT {
        hid_free_enumeration(seeker);
    };

    uint32_t firmware = 0x0;
    std::optional< std::string > dataPath;
    std::optional< std::string > requestPath;

    bool multipleWarned = false;
    while (seeker != nullptr) {
        if (seeker->interface_number == 1 && seeker->usage == 1
            && seeker->usage_page == 65280) {

            auto path = std::string(seeker->path);
            for (auto it = path.begin(); it != path.end(); it++) {
                *it = std::tolower(*it);
            }
            if (path.find(writeCol) != -1) {
                if (requestPath.has_value()) {
                    throw std::runtime_error(
                        "Multiple NuPhy Air75 keyboards found! Please keep only one plugged in.\n"
                    );
                }
                requestPath = seeker->path;
                firmware = seeker->release_number;
            } else if (path.find(dataCol) != -1) {
                if (dataPath.has_value()) {
                    throw std::runtime_error(
                        "Multiple NuPhy Air75 keyboards found! Please keep only one plugged in.\n"
                    );
                }
                dataPath = seeker->path;
            }
        }
        seeker = seeker->next;
    }

    if (dataPath.has_value() && requestPath.has_value()) {
        return Air75(dataPath.value(), firmware, requestPath);
    }

    return std::nullopt;
}
#else
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
                keyboard = Air75(seeker->path, seeker->release_number);
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

std::vector< uint32_t > Air75::getKeymap(bool mac) {
    auto handles = getHandles();
    SCOPE_EXIT {
        handles.cleanup(handles);
    };

    auto keymapReport = get_report(
        handles.data,
        handles.request,
        mac ? MAC_KEYMAP_GET_HEADER : WINDOWS_KEYMAP_GET_HEADER
    );
    // ALERT: Endianness-defined Behavior
    auto *start_pointer = (uint32_t *)&keymapReport[8];
    auto *end_pointer = (uint32_t *)(&keymapReport[0] + keymapReport.size());

    return std::vector< uint32_t >(start_pointer, end_pointer);
}

void Air75::setKeymap(const std::vector< uint32_t > &keymap, bool mac) {
    auto handles = getHandles();
    SCOPE_EXIT {
        handles.cleanup(handles);
    };

    auto header = WINDOWS_KEYMAP_WRITE_HEADER;
    size_t headerSize = sizeof WINDOWS_KEYMAP_WRITE_HEADER;
    if (mac) {
        header = MAC_KEYMAP_WRITE_HEADER;
        headerSize = sizeof MAC_KEYMAP_WRITE_HEADER;
    }

    size_t count = headerSize + (keymap.size() * 4);

    uint8_t *buffer = new uint8_t[count];
    SCOPE_EXIT {
        delete[] buffer;
    };

    // ALERT: Endianness-defined Behavior
    auto *start_pointer = (uint8_t *)&(*keymap.begin());
    auto *end_pointer = (uint8_t *)(&*keymap.begin() + keymap.size());

    std::copy(header, header + sizeof headerSize, buffer);

    std::copy(start_pointer, end_pointer, buffer + sizeof headerSize);
    set_report(handles.data, buffer, count);
}

const char *TOP_LEVEL_WIN = "keys";
const char *TOP_LEVEL_MAC = "mackeys";

void Air75::validateYAMLKeymap(
    const std::string &yamlString,
    bool rawOk,
    bool mac
) {
    auto config = YAML::Load(yamlString);

    auto topLevelKey = mac ? TOP_LEVEL_MAC : TOP_LEVEL_WIN;
    auto &indices =
        mac ? Air75::indicesByKeyNameMac : Air75::indicesByKeyNameWin;

    auto keys = config[topLevelKey];

    if (keys.Type() != YAML::NodeType::Map && !keys.IsNull()
        && keys.IsDefined()) {
        auto errorMessage =
            fmt::format("Invalid config file: '{}' is not a map.", topLevelKey);
        throw std::runtime_error(errorMessage);
    }

    if (keys.IsNull() || !keys.IsDefined()) {
        return;
    }

    for (auto entry : keys) {
        auto keyID = entry.first.as< std::string >();

        auto keyIt = indices.find(keyID);
        if (indices.find(keyID) == indices.end()) {
            auto errorMessage = fmt::format(
                "Invalid config in {}: a key for '{}' does not exist in '{}' mode.",
                topLevelKey,
                keyID,
                mac ? "Mac" : "Windows"
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
                    "Invalid config in {}.{}: raw configurations are not supported by the Nudelta GUI.",
                    topLevelKey,
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
                "Invalid config in {}.{}: a code for key '{}' was not found.",
                topLevelKey,
                keyID,
                codeID
            );
            throw std::runtime_error(errorMessage);
        }

        auto code = codeIt->second;
        auto modifiers = codeObject["modifiers"];
        if (modifiers.IsDefined() && !modifiers.IsNull()) {
            if (modifiers.Type() != YAML::NodeType::Sequence) {
                throw std::runtime_error(fmt::format(
                    "Invalid config in {}.{}: modifiers is not an array.",
                    topLevelKey,
                    keyID
                ));
            }
            for (auto modifier : modifiers) {
                auto modifierName = modifier.as< std::string >();
                auto modifierIt =
                    Air75::modifiersByModifierName.find(modifierName);
                if (modifierIt == Air75::modifiersByModifierName.end()) {
                    throw std::runtime_error(fmt::format(
                        "Invalid config in {}.{}: Unknown modifier {}: make sure you're not adding a direction, e.g. lalt instead of alt",
                        topLevelKey,
                        keyID,
                        modifierName
                    ));
                }
            }
        }
    }
}

void Air75::setKeymapFromYAML(const std::string &yamlString, bool mac) {
    validateYAMLKeymap(yamlString, true, false);
    validateYAMLKeymap(yamlString, true, true);

    auto config = YAML::Load(yamlString);

    auto writableKeymap =
        mac ? Air75::defaultKeymapMac : Air75::defaultKeymapWin;

    auto &indices =
        mac ? Air75::indicesByKeyNameMac : Air75::indicesByKeyNameWin;

    auto topLevelKey = mac ? TOP_LEVEL_MAC : TOP_LEVEL_WIN;
    auto keys = config[topLevelKey];

    if (!keys.IsNull() && keys.IsDefined()) {
        for (auto entry : keys) {
            auto keyID = entry.first.as< std::string >();

            auto keyIt = indices.find(keyID);
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
            if (modifiers.IsDefined() && !modifiers.IsNull()) {
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
    }

    setKeymap(writableKeymap, mac);
}

void Air75::resetKeymap(bool mac) {
    setKeymap(mac ? Air75::defaultKeymapMac : Air75::defaultKeymapWin, mac);
}