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

#include <napi.h>

using namespace Napi;

Napi::Value getKeyboardInfo(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    try {
        auto keyboard = NuPhy::find();
        if (keyboard == nullptr) {
            return env.Null();
        }

        auto string = fmt::format(
            "NuPhy {} (Firmware {:04x})",
            keyboard->getName(),
            keyboard->firmware
        );

        if (keyboard->dataPath == keyboard->requestPath
            && keyboard->dataPath.length() <= 20) {
            string = fmt::format("{} at {}", string, keyboard->dataPath);
        }

        auto object = Napi::Object::New(env);
        object["info"] = Napi::String::New(env, string);
        object["kind"] = Napi::String::New(env, keyboard->getName());

        return object;
    } catch (permissions_error &e) {
        auto error = Napi::Error::New(env, e.what());
        auto exception = error.Value();
        exception["kind"] = "Permission Error";
        napi_throw(env, exception);
    } catch (unsupported_keyboard &e) {
        auto error = Napi::Error::New(env, e.what());
        auto exception = error.Value();
        exception["kind"] = "Unsupported Keyboard";
        napi_throw(env, exception);
    }
    return env.Null();
}

Napi::Value validateYAML(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    try {
        if (info.Length() < 1 || !info[0].IsString()) {
            Napi::TypeError::New(
                env,
                "Internal error: validateYAML takes exactly one string argument"
            )
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        auto keyboard = NuPhy::find();
        if (keyboard == nullptr) {
            throw std::runtime_error("The keyboard was unplugged.");
        }

        auto keymapYAML = info[0].As< Napi::String >().Utf8Value();
        keyboard->validateYAMLKeymap(keymapYAML, false, false);
        keyboard->validateYAMLKeymap(keymapYAML, false, true);

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

        auto keyboard = NuPhy::find();
        if (keyboard == nullptr) {
            throw std::runtime_error("The keyboard was unplugged.");
        }

        auto keymapYAML = info[0].As< Napi::String >().Utf8Value();
        keyboard->setKeymapFromYAML(keymapYAML);
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