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
#include "defer.hpp"
#include "ssco.hpp"
#include "access.hpp"

#include <fstream>
#include <hidapi.h>
#include <iostream>
#include <yaml-cpp/yaml.h>

#define MAX_STR 255
#ifndef NUDELTA_VERSION
    #define NUDELTA_VERSION "UNKNOWN"
#endif

Air75 getKeyboard() {
    auto air75Optional = Air75::find();
    if (!air75Optional.has_value()) {
        throw std::runtime_error(
            "Couldn't find a NuPhy Air75 connected to this device"
        );
    }

    auto air75 = air75Optional.value();

    p("Found NuPhy Air75 at path {} (Firmware {:04x})\n",
      air75.path,
      air75.firmware);

    return air75;
}

SSCO_Fn(printVersion) {
    p("nudelta console v{}\n", NUDELTA_VERSION);
    p("Copyright (c) Mohamed Gaber 2022\n");
    p(R"(
Licensed under the GNU General Public License, version 3, or at your option,
any later version.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
)");
}

SSCO_Fn(printFirmware) {
    auto air75 = getKeyboard();
}

SSCO_Fn(resetKeymap) {
    auto air75 = getKeyboard();
    air75.resetKeymap();
    p("Wrote default keymap config to keyboard.\n");
}

SSCO_Fn(dumpKeymap) {
    auto air75 = getKeyboard();
    auto keys = air75.getKeymap();
    auto file = opts.options.find("dump-keys")->second;
    auto filePtr = fopen(file.c_str(), "wb");
    if (!filePtr) {
        throw std::runtime_error(
            fmt::format("Failed to open '{}' for writing", file)
        );
    }
    defer {
        fclose(filePtr);
    };

    // ALERT: Endianness-defined Behavior
    auto seeker = (char *)&*keys.begin();
    auto end = (char *)&*keys.end();
    fwrite(seeker, 1, end - seeker, filePtr);

    p("Wrote current keymap to '{}'.\n", file);

    auto hexFileIterator = opts.options.find("dump-hex-to");
    if (hexFileIterator != opts.options.end()) {
        auto hexFile = hexFileIterator->second;
        auto hexFilePtr = fopen(hexFile.c_str(), "w");
        if (!hexFilePtr) {
            throw std::runtime_error(
                fmt::format("Failed to open '{}' for writing", hexFile)
            );
        }
        defer {
            fclose(hexFilePtr);
        };

        prettyPrintBinary(
            std::vector< uint8_t >(
                (uint8_t *)&*keys.begin(),
                (uint8_t *)&*keys.end()
            ),
            hexFilePtr
        );

        p("Wrote current keymap in hex format to '{}'.\n", hexFile);
    }
}

SSCO_Fn(loadKeymap) {
    auto air75 = getKeyboard();
    auto keys = air75.getKeymap();
    auto file = opts.options.find("load-keys")->second;
    auto filePtr = fopen(file.c_str(), "rb");
    if (!filePtr) {
        throw std::runtime_error(
            fmt::format("Failed to open '{}' for reading", file)
        );
    }
    defer {
        fclose(filePtr);
    };

    uint8_t readBuffer[1024];
    fread(readBuffer, 1, sizeof readBuffer, filePtr);

    // ALERT: Endianness-defined Behavior
    auto keymap = std::vector< uint32_t >(
        (uint32_t *)readBuffer,
        (uint32_t *)(readBuffer + 1024)
    );

    air75.setKeymap(keymap);

    p("Wrote keymap '{}' to keyboard.", file);
}

SSCO_Fn(loadYAML) {
    auto air75 = getKeyboard();
    auto configPath = opts.options.find("load-profile")->second;

    std::string configStr;
    std::getline(std::ifstream(configPath), configStr, '\0');

    auto config = YAML::LoadFile(configPath);

    if (config["keys"]) {
        auto writableKeymap = Air75::defaultKeymap;
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
            auto codeID = codeObject["key"].as< std::string >();
            auto codeIt = Air75::keycodesByKeyName.find(codeID);
            if (Air75::keycodesByKeyName.find(codeID)
                == Air75::keycodesByKeyName.end()) {
                auto errorMessage = fmt::format(
                    "Invalid config file: a code for key '{}' was not found.\n",
                    codeID
                );
                throw std::runtime_error(errorMessage);
            }

            auto code = codeIt->second;
            auto modifiers = codeObject["modifiers"];
            if (modifiers.IsDefined() and !modifiers.IsNull()) {
                if (modifiers.Type() != YAML::NodeType::Sequence) {
                    throw std::runtime_error(fmt::format(
                        "Invalid config file: {}.modifiers is not an array.\n",
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
                    auto modifierCode = modifierIt->second;
                    code |= modifierCode;
                }
            }
            writableKeymap[key] = code;
        }
        air75.setKeymap(writableKeymap);

        p("Wrote keymap config in '{}' to keyboard.\n", configPath);
    }
}

int main(int argc, char *argv[]) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        p(stderr, "{}\n", hidAccessFailureMessage);
        return EX_NOPERM;
    }
    using Opt = SSCO::Option;

    SSCO::Options options(
        {Opt{"help",
             'h',
             "Show this message and exit.",
             false,
             [&](SSCO::Result &_) { options.printHelp(std::cout); }},
         Opt{"version",
             'V',
             "Show the current version of this app and exit.",
             false,
             printVersion},
         Opt{"firmware",
             'f',
             "Print the Air75 keyboard's firmware and exit.",
             false,
             printFirmware},
         Opt{"reset-keys",
             'r',
             "Restore the original keymap.",
             false,
             resetKeymap},
         Opt{"dump-keys",
             'D',
             "Dump the keymap to a binary file.",
             true,
             dumpKeymap},
         Opt{"dump-hex-to",
             'H',
             "When keys are dumped, also dump the keymap in a hex format to a text file.",
             true,
             std::nullopt},
         Opt{"load-keys",
             'L',
             "Load the keymap from a binary file.",
             true,
             loadKeymap},
         Opt{"load-profile", 'l', "Load YAML keymap", true, loadYAML}}
    );

    try {
        auto opts = options.process(argc, argv);

        if (opts.has_value()) {
            if (!opts.value().options.size()) {
                options.printHelp(std::cout);
            }
            return EX_OK;
        } else {
            options.printHelp(std::cout);
            return EX_USAGE;
        }
    } catch (std::runtime_error& e) {
        p(stderr, "{}\n", e.what());
        return EX_DATAERR;
    }

    return 0;
}