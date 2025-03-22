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
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class NuPhy { // Abstract
    public:
        std::string dataPath;
        std::string requestPath;
        uint16_t firmware;

        NuPhy(std::string dataPath, std::string requestPath, uint16_t firmware)
            : dataPath(dataPath), requestPath(requestPath), firmware(firmware) {
        }

        std::vector< uint32_t > getKeymap(bool mac = false);
        void setKeymap(const std::vector< uint32_t > &keymap, bool mac = false);
        void setKeymapFromYAML(const std::string &yamlString);
        void resetKeymap();

        virtual std::string getName() = 0;
        virtual std::vector< uint32_t > getDefaultKeymap(bool mac = false) = 0;
        virtual std::unordered_map< std::string, uint32_t >
        getIndicesByKeyName(bool mac = false) = 0;

        virtual std::unordered_map< std::string, uint32_t >
        getKeycodesByKeyName() {
            return keycodesByKeyName;
        }
        virtual std::unordered_map< std::string, uint32_t >
        getModifiersByModifierName() {
            return modifiersByModifierName;
        }

        virtual std::vector< uint8_t >
        getKeymapReportHeader(bool mac = false) = 0;
        virtual std::vector< uint8_t >
        setKeymapReportHeader(bool mac = false) = 0;

        static std::shared_ptr< NuPhy >
        find(bool verify = true); // Factory Method

        void validateYAMLKeymap(
            const std::string &yamlString,
            bool rawOk = true,
            bool mac = false
        );
        struct Handles {
                hid_device *data;
                hid_device
                    *request; // Same on macOS/Linux - different on Windows
                std::string dataPath;
                std::string requestPath;
                std::function< void(NuPhy::Handles &) > cleanup;
        };
    private:
        Handles getHandles();

        static const std::unordered_map< std::string, uint32_t >
            keycodesByKeyName;
        static const std::unordered_map< std::string, uint32_t >
            modifiersByModifierName;
};

class Air75 : public NuPhy {
    public:
        Air75(std::string dataPath, std::string requestPath, uint16_t firmware)
            : NuPhy(dataPath, requestPath, firmware) {}

        virtual std::string getName() { return "Air75"; }
        virtual std::vector< uint32_t > getDefaultKeymap(bool mac = false) {
            return mac ? Air75::defaultKeymapMac : Air75::defaultKeymapWin;
        }
        virtual std::unordered_map< std::string, uint32_t >
        getIndicesByKeyName(bool mac = false) {
            return mac ? Air75::indicesByKeyNameMac :
                         Air75::indicesByKeyNameWin;
        }
        virtual std::vector< uint8_t > getKeymapReportHeader(bool mac = false) {
            return mac ? std::vector< uint8_t >(
                       {0x05, 0x84, 0xd4, 0x00, 0x00, 0x00}
                   ) :
                         std::vector< uint8_t >(
                             {0x05, 0x84, 0xd8, 0x00, 0x00, 0x00}
                         );
        }
        virtual std::vector< uint8_t > setKeymapReportHeader(bool mac = false) {
            return mac ? std::vector< uint8_t >(
                       {0x06, 0x04, 0xd4, 0x00, 0x40, 0x00, 0x00, 0x00}
                   ) :
                         std::vector< uint8_t >(
                             {0x06, 0x04, 0xd8, 0x00, 0x40, 0x00, 0x00, 0x00}
                         );
        }
    private:
        static const std::vector< uint32_t > defaultKeymapWin;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameWin;

        static const std::vector< uint32_t > defaultKeymapMac;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameMac;
};

class Halo75 : public NuPhy {
    public:
        Halo75(std::string dataPath, std::string requestPath, uint16_t firmware)
            : NuPhy(dataPath, requestPath, firmware) {}

        virtual std::string getName() { return "Halo75"; }
        virtual std::vector< uint32_t > getDefaultKeymap(bool mac = false) {
            return mac ? Halo75::defaultKeymapMac : Halo75::defaultKeymapWin;
        }
        virtual std::unordered_map< std::string, uint32_t >
        getIndicesByKeyName(bool mac = false) {
            return mac ? Halo75::indicesByKeyNameMac :
                         Halo75::indicesByKeyNameWin;
        }

        virtual std::vector< uint8_t > getKeymapReportHeader(bool mac = false) {
            return mac ? std::vector< uint8_t >(
                       {0x05, 0x84, 0xd8, 0x00, 0x00, 0x00}
                   ) :
                         std::vector< uint8_t >(
                             {0x05, 0x84, 0xd4, 0x00, 0x00, 0x00}
                         );
        }
        virtual std::vector< uint8_t > setKeymapReportHeader(bool mac = false) {
            return mac ? std::vector< uint8_t >(
                       {0x06, 0x04, 0xd8, 0x00, 0x40, 0x00, 0x00, 0x00}
                   ) :
                         std::vector< uint8_t >(
                             {0x06, 0x04, 0xd4, 0x00, 0x40, 0x00, 0x00, 0x00}
                         );
        }
    private:
        static const std::vector< uint32_t > defaultKeymapWin;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameWin;

        static const std::vector< uint32_t > defaultKeymapMac;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameMac;
};

class Halo96 : public NuPhy {
    public:
        Halo96(std::string dataPath, std::string requestPath, uint16_t firmware)
            : NuPhy(dataPath, requestPath, firmware) {}

        virtual std::string getName() { return "Halo96"; }
        virtual std::vector< uint32_t > getDefaultKeymap(bool mac = false) {
            return mac ? Halo96::defaultKeymapMac : Halo96::defaultKeymapWin;
        }
        virtual std::unordered_map< std::string, uint32_t >
        getIndicesByKeyName(bool mac = false) {
            return mac ? Halo96::indicesByKeyNameMac :
                         Halo96::indicesByKeyNameWin;
        }

        // TODO: Confirm proper values
        virtual std::vector< uint8_t > getKeymapReportHeader(bool mac = false) {
            return mac ? std::vector< uint8_t >(
                       {0x05, 0x84, 0xd8, 0x00, 0x00, 0x00}
                   ) :
                         std::vector< uint8_t >(
                             {0x05, 0x84, 0xd4, 0x00, 0x00, 0x00}
                         );
        }

        // TODO: Confirm proper values
        virtual std::vector< uint8_t > setKeymapReportHeader(bool mac = false) {
            return mac ? std::vector< uint8_t >(
                       {0x06, 0x04, 0xd8, 0x00, 0x40, 0x00, 0x00, 0x00}
                   ) :
                         std::vector< uint8_t >(
                             {0x06, 0x04, 0xd4, 0x00, 0x40, 0x00, 0x00, 0x00}
                         );
        }
    private:
        static const std::vector< uint32_t > defaultKeymapWin;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameWin;

        static const std::vector< uint32_t > defaultKeymapMac;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyNameMac;
};

class unsupported_keyboard : public std::runtime_error {
    public:
        unsupported_keyboard(const std::string &what = "")
            : std::runtime_error(what) {}
};

#endif