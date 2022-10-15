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
#ifndef _common_h
#define _common_h

#include <codecvt>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <exception>
#include <fmt/core.h>
#include <locale>
#include <string>
#include <sysexits.h>
#include <vector>

std::string to_utf8(std::wstring in);
void prettyPrintBinary(const std::vector< uint8_t > &in, FILE *f = stdout);

#define p(...) fmt::print(__VA_ARGS__)

#ifdef _DEBUG
    #define d(...) fmt::print(stderr, __VA_ARGS__)
#else
    #define d(...) ;
#endif

#endif