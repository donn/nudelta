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
#include "common.hpp"

std::string to_utf8(std::wstring in) {
    return std::wstring_convert< std::codecvt_utf8< wchar_t > >().to_bytes(in);
}

void prettyPrintBinary(const std::vector< uint8_t > &in, FILE *f) {
    size_t offset = 0;
    for (auto element : in) {
        if (offset % 4 == 0) {
            p(f, "{:04x}  ", offset);
        }
        p(f, "{:02x} ", element);
        offset += 1;
        if (offset % 4 == 0) {
            p(f, "\n");
        }
    }
}
