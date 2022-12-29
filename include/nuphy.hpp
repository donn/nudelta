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
#ifndef _nuphy_hpp
#define _nuphy_hpp
#include "common.hpp"

#include <functional>
#include <hidapi.h>
#include <locale>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

class NuPhy { // Abstract
    public:
        uint16_t firmware;
        std::string path;
        std::optional< std::string > requestPath;

        NuPhy(
            std::string path,
            uint16_t firmware,
            std::optional< std::string > requestPath = std::nullopt
        )
            : path(path), firmware(firmware), requestPath(requestPath) {}


        std::vector< uint32_t > getKeymap(bool mac = false);
        void setKeymap(const std::vector< uint32_t > &keymap, bool mac = false);
        void setKeymapFromYAML(const std::string &yamlString);
        void resetKeymap();

        virtual std::string getName() = 0;
        virtual std::vector< uint32_t > getDefaultKeymap(bool mac = false) = 0;
        virtual std::unordered_map< std::string, uint32_t > getIndicesByKeyName(bool mac = false) = 0;
        virtual std::unordered_map< std::string, uint32_t > getKeycodesByKeyName() = 0;
        virtual std::unordered_map< std::string, uint32_t > getModifiersByModifierName() = 0;

        static std::shared_ptr<NuPhy> find(); // Factory Method

        void validateYAMLKeymap(
            const std::string &yamlString,
            bool rawOk = true,
            bool mac = false
        );
    private:
        struct Handles {
                hid_device *data;
                hid_device
                    *request; // Same on macOS/Linux - different on Windows
                std::function< void(NuPhy::Handles &) > cleanup;
        };
        Handles getHandles();
};

class Air75: public NuPhy {
public:
        Air75(
            std::string path,
            uint16_t firmware,
            std::optional< std::string > requestPath = std::nullopt
        )
            : NuPhy(path, firmware, requestPath) {}

        virtual std::string getName() {
            return "Air75";
        }
        virtual std::vector< uint32_t > getDefaultKeymap(bool mac = false) {
            return mac ? Air75::defaultKeymapMac : Air75::defaultKeymapWin;
        }
        virtual std::unordered_map< std::string, uint32_t > getIndicesByKeyName(bool mac = false) {
            return mac ? Air75::indicesByKeyNameMac : Air75::indicesByKeyNameWin;
        }
        virtual std::unordered_map< std::string, uint32_t > getKeycodesByKeyName() {
            return keycodesByKeyName;
        }
        virtual std::unordered_map< std::string, uint32_t > getModifiersByModifierName() {
            return modifiersByModifierName;
        }
private:
        static const std::vector< uint32_t > defaultKeymapWin;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameWin;

        static const std::vector< uint32_t > defaultKeymapMac;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameMac;

        static const std::unordered_map< std::string, uint32_t >
            keycodesByKeyName;
        static const std::unordered_map< std::string, uint32_t >
            modifiersByModifierName;
};

#endif