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
#include "nuphy.hpp"

#include "access.hpp"

#include <algorithm>
#include <scope_guard.hpp>
#include <sstream>
#include <yaml-cpp/yaml.h>

NuPhy::Handles NuPhy::getHandles() {
    auto dataHandle = hid_open_path(dataPath.c_str());
    auto requestHandle = dataHandle;
    if (requestPath != dataPath) {
        requestHandle = hid_open_path(requestPath.c_str());
    }

    if (dataHandle == nullptr || requestHandle == nullptr) {
        throw permissions_error(hidAccessFailureMessage);
    }

    auto cleanup = [](NuPhy::Handles &handles) {
        if (handles.request != handles.data) {
            hid_close(handles.request);
        }
        hid_close(handles.data);
    };

    return {dataHandle, requestHandle, dataPath, requestPath, cleanup};
}

static const size_t MAX_READABLE_SIZE = 0x7FF;

static const uint8_t REQUEST_0[] = {0x05, 0x83, 0xb6, 0x00, 0x00, 0x00};
static const uint8_t REQUEST_1[] = {0x05, 0x88, 0xb8, 0x00, 0x00, 0x00};

static int get_report(
    NuPhy::Handles handles,
    const uint8_t *requestInfo,
    const size_t requestSize,
    uint8_t *readBuffer
) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        throw permissions_error(hidAccessFailureMessage);
    }

    auto bytesWritten =
        hid_send_feature_report(handles.request, requestInfo, requestSize);
    if (bytesWritten < 0) {
        auto errorString = fmt::format(
            "Failed to write to keyboard: {}",
            to_utf8(hid_error(handles.request))
        );
        throw std::runtime_error(errorString);
    } else {
        d("Wrote {} bytes.\n", bytesWritten);
    }
    readBuffer[0] = 0x06;
    auto bytesRead =
        hid_get_feature_report(handles.data, readBuffer, MAX_READABLE_SIZE);
    if (bytesRead < 0) {
        auto errorString = fmt::format(
            "Failed to read from keyboard: {}",
            to_utf8(hid_error(handles.data))
        );
        throw std::runtime_error("Failed to read from keyboard");
    } else {
        d("Read {} bytes.\n", bytesRead);
    }

    return bytesRead;
}

static std::vector< uint8_t > get_report(
    NuPhy::Handles handles,
    const uint8_t *requestInfo,
    size_t requestSize
) {
    uint8_t readBuffer[MAX_READABLE_SIZE];

    auto read = get_report(handles, requestInfo, requestSize, readBuffer);

    return std::vector< uint8_t >(readBuffer, readBuffer + read);
}

void set_report(NuPhy::Handles handles, uint8_t *data, size_t dataSize) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        throw std::runtime_error(hidAccessFailureMessage);
    }
    auto bytesWritten = hid_send_feature_report(handles.data, data, dataSize);
    if (bytesWritten < 0) {
        auto errorString = fmt::format(
            "Failed to write to keyboard: {}",
            to_utf8(hid_error(handles.request))
        );
        throw std::runtime_error(errorString);
    } else {
        d("Wrote {} bytes.\n", bytesWritten);
    }
}

static std::shared_ptr< NuPhy > createKeyboard(
    std::string name,
    std::string dataPath,
    std::string requestPath,
    uint16_t firmware,
    bool verify = true
) {
    if (name == "Air75" || !verify) {
        return std::make_shared< Air75 >(dataPath, requestPath, firmware);
    }
    if (name == "NuPhy Halo75") {
        return std::make_shared< Halo75 >(dataPath, requestPath, firmware);
    }
    if (name == "NuPhy Halo96") {
        return std::make_shared< Halo96 >(dataPath, requestPath, firmware);
    }
    return nullptr;
}

std::string represent_hid_struct(hid_device_info *info) {
    std::stringstream str;

    str << std::hex;

    str << "At path: " << info->path << std::endl;
    str << "VID/PID: " << info->vendor_id << ":" << info->product_id
        << std::endl;
    if (info->serial_number != nullptr) {
        str << "SN: " << info->serial_number << std::endl;
    }
    str << "Release: " << info->release_number << std::endl;
    if (info->manufacturer_string != nullptr) {
        str << "Manufacturer String: " << to_utf8(info->manufacturer_string)
            << std::endl;
    }
    if (info->product_string != nullptr) {
        str << "Product String: " << to_utf8(info->product_string) << std::endl;
    }
    str << "Usage/Page: " << info->usage << "/" << info->usage_page
        << std::endl;
    str << "Interface Number: " << std::dec << info->interface_number
        << std::endl;
    str << "---" << std::endl;
    return str.str();
}

#ifdef _WIN32
// Win is different: there are multiple "channels" each with a different
// path
// - So far, I've identified col06 as the one for keymap data, col05 for
// requests
std::string requestCol = "col05";
std::string dataCol = "col06";

std::shared_ptr< NuPhy > NuPhy::find(bool verify) {
    auto seeker = hid_enumerate(0x05ac, 0x024f);
    SCOPE_EXIT {
        hid_free_enumeration(seeker);
    };

    uint16_t firmware = 0x0;
    std::string manufacturerString = "";
    std::optional< std::string > productName;
    std::optional< std::string > dataPath;
    std::optional< std::string > requestPath;

    while (seeker != nullptr) {
        if (seeker->interface_number != -1 && seeker->usage == 1
            && seeker->usage_page == 0xFF00
            && (!productName.has_value()
                || productName.value() == to_utf8(seeker->product_string))) {

            auto path = std::string(seeker->path);
            for (auto it = path.begin(); it != path.end(); it++) {
                *it = char(std::tolower(*it));
            }
            if (path.find(requestCol) != -1) {
                if (requestPath.has_value()) {
                    throw std::runtime_error(
                        "Multiple keyboards with the same product ID found! Please ensure only one keyboard is plugged in.\n"
                    );
                }
                productName = to_utf8(seeker->product_string);
                requestPath = seeker->path;
                firmware = seeker->release_number;
                manufacturerString = to_utf8(seeker->manufacturer_string);
            } else if (path.find(dataCol) != -1) {
                if (dataPath.has_value()) {
                    throw std::runtime_error(
                        "Multiple keyboards with the same product ID found! Please ensure only one keyboard is plugged in.\n"
                    );
                }
                productName = to_utf8(seeker->product_string);
                dataPath = seeker->path;
            }
        }
        seeker = seeker->next;
    }

    if (dataPath.has_value() && requestPath.has_value()) {
        auto keyboard = createKeyboard(
            productName.value(),
            dataPath.value(),
            requestPath.value(),
            firmware,
            verify
        );
        if (keyboard == nullptr) {
            throw unsupported_keyboard(fmt::format(
                "No supported keyboards found, but a similar keyboard, '{} {}', has been found.\n\nIf you believe this keyboard not being supported is an error, please file a bug report.",
                manufacturerString,
                productName.value()
            ));
        }
        return keyboard;
    }

    return nullptr;
}
#else
std::shared_ptr< NuPhy > NuPhy::find(bool verify) {
    std::shared_ptr< NuPhy > keyboard;

    auto seeker = hid_enumerate(0x05ac, 0x024f);
    SCOPE_EXIT {
        hid_free_enumeration(seeker);
    };

    bool unsupportedDetected = false;
    bool multipleWarned = false;
    std::string productString = "";
    while (seeker != nullptr) {
        if (seeker->interface_number != -1 && seeker->usage == 1
            && seeker->usage_page == 0xFF00) {
            if (keyboard != nullptr) {
                // We only care if the path is different, because that means a
                // different device on Mac and Linux
                if (keyboard->dataPath != std::string(seeker->path)) {
                    if (!multipleWarned) {
                        p(stderr,
                          "[Warning] Multiple NuPhy keyboards found! Please keep only one plugged in. Only the first matched device will be used.\n"
                        );
                        multipleWarned = true;
                    }
                }
            } else {
                if (seeker->product_string == nullptr) {
                    throw permissions_error(hidAccessFailureMessage);
                }
                auto productName = to_utf8(seeker->product_string);
                productString = productName;
                if (auto manufacturerStringW = seeker->manufacturer_string) {
                    // There is no manufacturerString on the Linux/libusb
                    // implementation.
                    auto manufacturerName = to_utf8(manufacturerStringW);
                    productString =
                        fmt::format("{} {}", manufacturerName, productName);
                }
                keyboard = createKeyboard(
                    productName,
                    seeker->path,
                    seeker->path,
                    seeker->release_number,
                    verify
                );
                if (keyboard == nullptr) {
                    unsupportedDetected = true;
                }
            }
        }
        seeker = seeker->next;
    }

    if (keyboard == nullptr && unsupportedDetected) {
        throw unsupported_keyboard(fmt::format(
            "No supported keyboards found, but a similar keyboard, '{}', has been found.\n\nIf you believe this keyboard not being supported is an error, please file a bug report.",
            productString
        ));
    }

    return keyboard;
}
#endif

std::vector< uint32_t > NuPhy::getKeymap(bool mac) {
    auto handles = getHandles();
    SCOPE_EXIT {
        handles.cleanup(handles);
    };

    auto requestHeader = getKeymapReportHeader(mac);

    auto keymapReport =
        get_report(handles, requestHeader.data(), requestHeader.size());
    // ALERT: Endianness-defined Behavior
    auto *start_pointer = (uint32_t *)&keymapReport[8];
    auto *end_pointer = (uint32_t *)(keymapReport.data() + keymapReport.size());

    return std::vector< uint32_t >(start_pointer, end_pointer);
}

void NuPhy::setKeymap(const std::vector< uint32_t > &keymap, bool mac) {
    auto handles = getHandles();
    SCOPE_EXIT {
        handles.cleanup(handles);
    };

    auto header = setKeymapReportHeader(mac);

    size_t count = header.size() + (keymap.size() * 4);

    uint8_t *buffer = new uint8_t[count];
    SCOPE_EXIT {
        delete[] buffer;
    };

    // ALERT: Endianness-defined Behavior
    auto *start_pointer = (uint8_t *)keymap.data();
    auto *end_pointer = (uint8_t *)(keymap.data() + keymap.size());

    std::copy(header.data(), header.data() + header.size(), buffer);
    std::copy(start_pointer, end_pointer, buffer + header.size());

    set_report(handles, buffer, count);
}

const char *TOP_LEVEL_WIN = "keys";
const char *TOP_LEVEL_MAC = "mackeys";

void NuPhy::validateYAMLKeymap(
    const std::string &yamlString,
    bool rawOk,
    bool mac
) {
    auto config = YAML::Load(yamlString);

    auto keycodes = getKeycodesByKeyName();
    auto modifiersByName = getModifiersByModifierName();
    auto writableKeymap = getDefaultKeymap(mac);
    auto indices = getIndicesByKeyName(mac);

    auto topLevelKey = mac ? TOP_LEVEL_MAC : TOP_LEVEL_WIN;

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

        if (indices.find(keyID) == indices.end()) {
            auto errorMessage = fmt::format(
                "Invalid config in {}: a key for '{}' does not exist in '{}' mode.",
                topLevelKey,
                keyID,
                mac ? "Mac" : "Windows"
            );
            throw std::runtime_error(errorMessage);
        }

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
        if (keycodes.find(codeID) == keycodes.end()) {
            auto errorMessage = fmt::format(
                "Invalid config in {}.{}: a code for key '{}' was not found.",
                topLevelKey,
                keyID,
                codeID
            );
            throw std::runtime_error(errorMessage);
        }

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
                auto modifierIt = modifiersByName.find(modifierName);
                if (modifierIt == modifiersByName.end()) {
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

void NuPhy::setKeymapFromYAML(const std::string &yamlString) {
    validateYAMLKeymap(yamlString, true, false);
    validateYAMLKeymap(yamlString, true, true);

    auto config = YAML::Load(yamlString);

    auto keycodes = getKeycodesByKeyName();
    auto modifiersByName = getModifiersByModifierName();

    for (auto mac : {true, false}) {
        auto writableKeymap = getDefaultKeymap(mac);
        auto indices = getIndicesByKeyName(mac);

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
                auto codeIt = keycodes.find(codeID);

                auto code = codeIt->second;
                auto modifiers = codeObject["modifiers"];
                if (modifiers.IsDefined() && !modifiers.IsNull()) {
                    for (auto modifier : modifiers) {
                        auto modifierName = modifier.as< std::string >();
                        auto modifierIt = modifiersByName.find(modifierName);
                        auto modifierCode = modifierIt->second;
                        code |= modifierCode;
                    }
                }
                writableKeymap[key] = code;
            }
        }

        setKeymap(writableKeymap, mac);
    }
}

void NuPhy::resetKeymap() {
    setKeymap(getDefaultKeymap(false), false);
    setKeymap(getDefaultKeymap(true), true);
}