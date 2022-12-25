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
#include "air75.hpp"

#include <napi.h>

using namespace Napi;

Napi::Value getKeyboardInfo(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    try {
        auto air75Optional = Air75::find();
        if (!air75Optional.has_value()) {
            return env.Null();
        }

        auto air75 = air75Optional.value();

        auto string =
            fmt::format("NuPhy Air75 (Firmware {:04x})", air75.firmware);

        if (air75.path.length() <= 20) {
            string = fmt::format("{} at {}", string, air75.path);
        }

        return Napi::String::New(env, string);
    } catch (permissions_error &e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Null();
}

Napi::Value validateYAML(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    try {
        if (info.Length() < 1) {
            Napi::TypeError::New(
                env,
                "Internal error: validateYAML takes exactly one argument"
            )
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString()) {
            Napi::TypeError::New(
                env,
                "Internal error: validateYAML takes exactly one string argument"
            )
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        auto keymapYAML = info[0].As< Napi::String >().Utf8Value();
        Air75::validateYAMLKeymap(keymapYAML, false, false);
        Air75::validateYAMLKeymap(keymapYAML, false, true);

    } catch (std::runtime_error &e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Null();
}

Napi::Value setKeymapFromYAML(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    try {
        if (info.Length() < 1) {
            Napi::TypeError::New(
                env,
                "Internal error: setKeymapFromYAML takes exactly one argument"
            )
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString()) {
            Napi::TypeError::New(
                env,
                "Internal error: setKeymapFromYAML takes exactly one string argument"
            )
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        auto air75Optional = Air75::find();
        if (!air75Optional.has_value()) {
            throw std::runtime_error("The keyboard was unplugged.");
        }

        auto air75 = air75Optional.value();

        auto keymapYAML = info[0].As< Napi::String >().Utf8Value();
        air75.setKeymapFromYAML(keymapYAML);
        air75.setKeymapFromYAML(keymapYAML, true);
    } catch (std::runtime_error &e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    }
    return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(
        Napi::String::New(env, "getKeyboardInfo"),
        Napi::Function::New(env, getKeyboardInfo)
    );
    exports.Set(
        Napi::String::New(env, "validateYAML"),
        Napi::Function::New(env, validateYAML)
    );
    exports.Set(
        Napi::String::New(env, "setKeymapFromYAML"),
        Napi::Function::New(env, setKeymapFromYAML)
    );
    return exports;
}

NODE_API_MODULE(addon, Init)