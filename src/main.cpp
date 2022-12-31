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
#include "access.hpp"
#include "nuphy.hpp"

#include <fstream>
#include <hidapi.h>
#include <iostream>
#include <scope_guard.hpp>
#include <ssco.hpp>

#define MAX_STR 255
#ifndef NUDELTA_VERSION
    #define NUDELTA_VERSION "UNKNOWN"
#endif

std::shared_ptr< NuPhy > getKeyboard() {
    auto keyboard = NuPhy::find();
    if (keyboard == nullptr) {
        throw std::runtime_error(
            "Couldn't find a NuPhy keyboard connected to this device. Make sure it's plugged in via USB."
        );
    }

    if (keyboard->dataPath == keyboard->requestPath) {
        p("Found NuPhy {} at path {} (Firmware {:04x})\n",
          keyboard->getName(),
          keyboard->dataPath,
          keyboard->firmware);
    } else {
        p("Found NuPhy {} at paths ({}, {}) (Firmware {:04x})\n",
          keyboard->getName(),
          keyboard->dataPath,
          keyboard->requestPath,
          keyboard->firmware);
    }

    return keyboard;
}

SSCO_Fn(printVersion) {
    p("Nudelta Utility v{}\n", NUDELTA_VERSION);
    p("Copyright (c) Mohamed Gaber 2022\n");
    p(R"(
Licensed under the GNU General Public License, version 3, or at your option,
any later version.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
)");
}

SSCO_Fn(printFirmware) {
    auto keyboard = getKeyboard();
}

SSCO_Fn(resetKeymap) {
    auto keyboard = getKeyboard();
    keyboard->resetKeymap();
    p("Wrote default keymap config to the keyboard's Windows and Mac modes.\n");
}

SSCO_Fn(dumpKeymap) {
    auto mac = opts.options.find("mac") != opts.options.end();

    auto keyboard = getKeyboard();
    auto keys = keyboard->getKeymap(mac);
    auto file = opts.options.find("dump-keys")->second;
    auto filePtr = fopen(file.c_str(), "wb");

    if (!filePtr) {
        throw std::runtime_error(
            fmt::format("Failed to open '{}' for writing", file)
        );
    }

    SCOPE_EXIT {
        fclose(filePtr);
    };

    // ALERT: Endianness-defined Behavior
    auto seeker = (char *)keys.data();
    auto end = (char *)(keys.data() + keys.size());
    fwrite(seeker, 1, end - seeker, filePtr);

    p("Wrote current {} keymap to '{}'.\n", mac ? "Mac" : "Windows", file);

    auto hexFileIterator = opts.options.find("dump-hex-to");
    if (hexFileIterator != opts.options.end()) {
        auto hexFile = hexFileIterator->second;
        auto hexFilePtr = fopen(hexFile.c_str(), "w");
        if (!hexFilePtr) {
            throw std::runtime_error(
                fmt::format("Failed to open '{}' for writing", hexFile)
            );
        }
        SCOPE_EXIT {
            fclose(hexFilePtr);
        };

        prettyPrintBinary(
            std::vector< uint8_t >(
                (uint8_t *)keys.data(),
                (uint8_t *)(keys.data() + keys.size())
            ),
            hexFilePtr
        );

        p("Wrote current keymap in hex format to '{}'.\n", hexFile);
    }
}

SSCO_Fn(loadKeymap) {
    auto mac = opts.options.find("mac") != opts.options.end();

    auto keyboard = getKeyboard();
    auto keys = keyboard->getKeymap(mac);
    auto file = opts.options.find("load-keys")->second;
    auto filePtr = fopen(file.c_str(), "rb");
    if (!filePtr) {
        throw std::runtime_error(
            fmt::format("Failed to open '{}' for reading", file)
        );
    }
    SCOPE_EXIT {
        fclose(filePtr);
    };

    uint8_t readBuffer[1024];
    fread(readBuffer, 1, sizeof readBuffer, filePtr);

    // ALERT: Endianness-defined Behavior
    auto keymap = std::vector< uint32_t >(
        (uint32_t *)readBuffer,
        (uint32_t *)(readBuffer + 1024)
    );

    keyboard->setKeymap(keymap, mac);

    p("Wrote keymap '{}' to the keyboard's {} mode.\n",
      file,
      mac ? "Mac" : "Windows");
}

SSCO_Fn(loadYAML) {
    auto keyboard = getKeyboard();
    auto configPath = opts.options.find("load-profile")->second;

    std::string configStr;
    std::getline(std::ifstream(configPath), configStr, '\0');
    keyboard->setKeymapFromYAML(configStr);

    p("Wrote keymap '{}' to the keyboard.\n", configPath);
}

int main(int argc, char *argv[]) {
    auto hidAccess = checkHIDAccess();
    if (!hidAccess.has_value()) {
        hidAccess = requestHIDAccess();
    }
    if (!hidAccess.value()) {
        p(stderr, "{}\n", hidAccessFailureMessage);
        return -1;
    }
    using Opt = SSCO::Option;

    SSCO::Options options(
        {Opt{"version",
             'V',
             "Show the current version of this app and exit.",
             false,
             printVersion},
         Opt{"firmware",
             'f',
             "Print the connected keyboard's firmware and exit.",
             false,
             printFirmware},
         Opt{"mac",
             'M',
             "Valid only if dump-keys or load-keys are passed: operate on the Mac mode of the keyboard instead of the Win mode.",
             false},
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
             "When the keymap is dumped to a binary file, also dump the keymap in a hex format to a text file.",
             true},
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
            return 0;
        } else {
            options.printHelp(std::cout);
            return 1;
        }
    } catch (std::runtime_error &e) {
        p(stderr, "[ERROR] {}\n", e.what());
        return -1;
    }

    return 0;
}