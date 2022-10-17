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
#ifndef _air75_hpp
#define _air75_hpp
#include "common.hpp"
#include "defer.hpp"

#include <hidapi.h>
#include <locale>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Air75 {
    public:
        enum class Key {
            escape = 0,
            grave = 1,
            caps_lock = 3,
            lshift = 4,
            lctrl = 5,
            lalt = 17,
            ralt = 53,
            rctrl = 65,

            tab = 2,
            space = 35,
            backspace = 79,

            num1 = 145,
            num2 = 146,
            num3 = 147,
            num4 = 148,
            num5 = 31,
            num6 = 37,
            num7 = 43,
            num8 = 49,
            num9 = 55,
            num0 = 61,

            a = 9,
            b = 34,
            c = 22,
            d = 21,
            e = 20,
            f = 166,
            g = 162,
            h = 39,
            i = 50,
            j = 45,
            k = 51,
            l = 57,
            m = 46,
            n = 11,
            o = 12,
            p = 62,
            q = 157,
            r = 26,
            s = 163,
            t = 32,
            u = 18,
            v = 28,
            w = 158,
            x = 16,
            y = 38,
            z = 10,

            semicolon = 63,
            minus = 67,
            equal = 73,
            quote = 69,
            lbracket = 68,
            rbracket = 159,
            backslash = 160,
            comma = 164,
            period = 165,

            pgup = 91,
            pgdn = 92,
            home = 93,
            end = 94,
            del = 209,
        };

        uint16_t firmware;
        std::string productString;
        std::string path;

        Air75(std::wstring productString, std::string path, uint16_t firmware)
            : productString(to_utf8(productString)), path(path),
              firmware(firmware) {}

        std::vector< uint32_t > getKeymap();
        void setKeymap(const std::vector< uint32_t > &keymap);
        void resetKeymap();

        static std::optional< Air75 > find();

        static const std::vector< uint32_t > defaultKeymap;
        static const std::unordered_map< std::string, uint32_t >
            indicesByKeyName;
        static const std::unordered_map< std::string, uint32_t >
            keycodesByKeyName;
        static const std::unordered_map< std::string, uint32_t >
            modifiersByModifierName;
    private:
        std::vector< uint8_t > request0();
        std::vector< uint8_t > request1();

        hid_device *handle();
};

#endif