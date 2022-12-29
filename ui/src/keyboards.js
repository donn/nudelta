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
import air75_keycodes from "../../res/Air75/keycodes.yml";
import halo75_keycodes from "../../res/Halo75/keycodes.yml";

class Key {
    constructor({
        id,
        color,
        label,
        width,
        name,
        remappable,
        defaultMapping,
        defaultModifiers,
        altLabel,
        altID,
        altName,
        altDefaultMapping,
        altDefaultModifiers,
    }) {
        this.id = id;
        this.color = color ?? "white";
        this.label = label ?? this.id.toUpperCase();
        this.width = width ?? 1.0;
        this.name = name ?? this.id.charAt(0).toUpperCase() + this.id.slice(1);
        this.remappable = remappable ?? true;
        this.defaultMapping = defaultMapping ?? this.id;
        this.defaultModifiers = defaultModifiers ?? [];

        this.altLabel = altLabel ?? null;
        this.altID = altID ?? null;
        this.altName = altName ?? null;
        this.altDefaultMapping = altDefaultMapping ?? this.id;
        this.altDefaultModifiers = altDefaultModifiers ?? this.defaultModifiers;
    }
}

export const Air75 = {
    keycodes: air75_keycodes,
    getLayout(mode) {
        let mac = mode === "mac";
        return [
            [
                new Key({ id: "esc", color: "mint", name: "Escape" }),
                new Key({
                    id: "f1",

                    altLabel: " ",
                    altID: mac ? "fn_f1" : "brightnessdown",
                    altName: mac ? "F1 (Fn layer)" : "Brightness Down",
                    altDefaultMapping: mac ? "f1" : "brightnessdown",
                }),
                new Key({
                    id: "f2",

                    altLabel: " ",
                    altID: mac ? "fn_f2" : "brightnessup",
                    altName: mac ? "F2 (Fn layer)" : "Brightness Up",
                    altDefaultMapping: mac ? "f2" : "brightnessup",
                }),
                new Key({
                    id: "f3",

                    altLabel: " ",
                    altID: mac ? "fn_f3" : "missioncontrol",
                    altName: mac ? "F3 (Fn layer)" : "Mission Control",
                    altDefaultMapping: mac ? "f3" : "none",
                }),
                new Key({
                    id: "f4",

                    altLabel: " ",
                    altID: mac ? "fn_f4" : "launchpad",
                    altName: mac ? "F4 (Fn layer)" : "Launchpad",
                    altDefaultMapping: mac ? "f4" : "none",
                }),
                new Key({
                    id: "f5",
                    color: "gray",
                    defaultMapping: mac ? "backlightdown" : null,

                    altLabel: " ",
                    altID: mac ? "fn_f5" : "backlightdown",
                    altName: mac ? "F5 (Fn layer)" : "Backlight Down",
                    altDefaultMapping: mac ? "f5" : "backlightdown",
                }),
                new Key({
                    id: "f6",
                    color: "gray",
                    defaultMapping: mac ? "backlightup" : null,

                    altLabel: " ",
                    altID: mac ? "fn_f6" : "backlightup",
                    altName: mac ? "F6 (Fn layer)" : "Backlight Up",
                    altDefaultMapping: mac ? "f6" : "backlightup",
                }),
                new Key({
                    id: "f7",
                    color: "gray",

                    altLabel: " ",
                    altID: mac ? "fn_f7" : "rewind",
                    altName: mac ? "F7 (Fn layer)" : "Rewind",
                    altDefaultMapping: mac ? "f7" : "rewind",
                }),
                new Key({
                    id: "f8",
                    color: "gray",

                    altLabel: " ",
                    altID: mac ? "fn_f8" : "playpause",
                    altName: mac ? "F8 (Fn layer)" : "Play/Pause Toggle",
                    altDefaultMapping: mac ? "f8" : "playpause",
                }),
                new Key({
                    id: "f9",

                    altLabel: " ",
                    altID: mac ? "fn_f9" : "forward",
                    altName: mac ? "F9 (Fn layer)" : "Fast-Forward",
                    altDefaultMapping: mac ? "f9" : "forward",
                }),
                new Key({
                    id: "f10",

                    altLabel: " ",
                    altID: mac ? "fn_f10" : "mute",
                    altName: mac ? "F10 (Fn layer)" : "Mute",
                    altDefaultMapping: mac ? "f10" : "mute",
                }),
                new Key({
                    id: "f11",

                    altLabel: " ",
                    altID: mac ? "fn_f11" : "volumedown",
                    altName: mac ? "F11 (Fn layer)" : "Volume Down",
                    altDefaultMapping: mac ? "f11" : "volumedown",
                }),
                new Key({
                    id: "f12",

                    altLabel: " ",
                    altID: mac ? "fn_f12" : "volumeup",
                    altName: mac ? "F12 (Fn layer)" : "Volume Up",
                    altDefaultMapping: mac ? "f12" : "volumeup",
                }),
                new Key({
                    id: "screenshot",
                    color: "gray",
                    label: "✂️",
                    defaultMapping: mac ? "num4" : "s",
                    defaultModifiers: ["meta", "shift"],
                }),
                new Key({
                    id: "assistant",
                    color: "gray",
                    label: "🐱",
                    defaultMapping: mac ? "fnspace" : "c",
                    defaultModifiers: mac ? [] : ["meta"],
                }),
                new Key({ id: "del", color: "gray", name: "Delete" }),
            ],
            [
                new Key({
                    id: "grave",
                    label: "`",
                    color: "gray",
                    altLabel: "~",
                }),
                new Key({ id: "num1", label: "1", altLabel: "!" }),
                new Key({ id: "num2", label: "2", altLabel: "@" }),
                new Key({ id: "num3", label: "3", altLabel: "#" }),
                new Key({ id: "num4", label: "4", altLabel: "$" }),
                new Key({ id: "num5", label: "5", altLabel: "%" }),
                new Key({ id: "num6", label: "6", altLabel: "^" }),
                new Key({ id: "num7", label: "7", altLabel: "&" }),
                new Key({ id: "num8", label: "8", altLabel: "*" }),
                new Key({ id: "num9", label: "9", altLabel: "(" }),
                new Key({ id: "num0", label: "0", altLabel: ")" }),
                new Key({ id: "minus", label: "-", altLabel: "_" }),
                new Key({ id: "equal", label: "=", altLabel: "+" }),
                new Key({ id: "backspace", width: 2.0, color: "gray" }),
                new Key({ id: "pgup", color: "gray", name: "Page Up" }),
            ],
            [
                new Key({ id: "tab", width: 1.5, color: "gray" }),
                new Key({ id: "q" }),
                new Key({ id: "w" }),
                new Key({ id: "e" }),
                new Key({ id: "r" }),
                new Key({ id: "t" }),
                new Key({ id: "y" }),
                new Key({ id: "u" }),
                new Key({ id: "i" }),
                new Key({ id: "o" }),
                new Key({ id: "p" }),
                new Key({
                    id: "rbracket",
                    label: "[",
                    name: "Left Bracket",
                    altLabel: "{",
                }),
                new Key({
                    id: "lbracket",
                    label: "]",
                    name: "Right Bracket",
                    altLabel: "}",
                }),
                new Key({
                    id: "backslash",
                    label: "\\",
                    width: 1.5,
                    name: "Backslash",
                    altLabel: "|",
                }),
                new Key({ id: "pgdn", color: "gray", name: "Page Down" }),
            ],
            [
                new Key({
                    id: "capslock",
                    label: "CAPS",
                    name: "Caps Lock",
                    width: 1.75,
                    color: "gray",
                }),
                new Key({ id: "a" }),
                new Key({ id: "s" }),
                new Key({ id: "d" }),
                new Key({ id: "f" }),
                new Key({ id: "g" }),
                new Key({ id: "h" }),
                new Key({ id: "j" }),
                new Key({ id: "k" }),
                new Key({ id: "l" }),
                new Key({
                    id: "semicolon",
                    label: ";",
                    name: "Semicolon",
                    altLabel: ":",
                }),
                new Key({
                    id: "quote",
                    label: "'",
                    name: "Quote",
                    altLabel: '"',
                }),
                new Key({ id: "enter", width: 2.25, color: "orange" }),
                new Key({ id: "home", color: "gray" }),
            ],
            [
                new Key({
                    id: "lshift",
                    label: "SHIFT",
                    name: "Left Shift",
                    width: 2.25,
                    color: "gray",
                }),
                new Key({ id: "z" }),
                new Key({ id: "x" }),
                new Key({ id: "c" }),
                new Key({ id: "v" }),
                new Key({ id: "b" }),
                new Key({ id: "n" }),
                new Key({ id: "m" }),
                new Key({
                    id: "comma",
                    label: ",",
                    name: "Comma",
                    altLabel: "<",
                }),
                new Key({
                    id: "period",
                    label: ".",
                    name: "Period",
                    altLabel: ">",
                }),
                new Key({
                    id: "fwdslash",
                    label: "/",
                    name: "Slash",
                    altLabel: "?",
                }),
                new Key({
                    id: "rshift",
                    label: "SHIFT",
                    name: "Right Shift",
                    width: 1.75,
                    color: "gray",
                }),
                new Key({
                    id: "up",
                    label: "˄",
                    name: "Up Arrow",
                    color: "gray",
                }),
                new Key({ id: "end", color: "gray" }),
            ],
            [
                new Key({
                    id: "lctrl",
                    label: "CTRL",
                    name: "Left Control",
                    width: 1.25,
                    color: "gray",
                }),
                mac
                    ? new Key({
                          id: "lalt",
                          label: "OPT",
                          name: "Left Alt/Option",
                          width: 1.25,
                          color: "gray",
                      })
                    : new Key({
                          id: "lmeta",
                          label: "WIN",
                          name: "Left Meta",
                          width: 1.25,
                          color: "gray",
                      }),
                mac
                    ? new Key({
                          id: "lmeta",
                          label: "CMD",
                          name: "Left Meta",
                          width: 1.25,
                          color: "gray",
                      })
                    : new Key({
                          id: "lalt",
                          label: "ALT",
                          name: "Left Alt/Option",
                          width: 1.25,
                          color: "gray",
                      }),
                new Key({
                    id: "space",
                    label: "",
                    name: "Spacebar",
                    width: 6.25,
                    color: "yellow",
                }),
                mac
                    ? new Key({
                          id: "rmeta",
                          label: "CMD",
                          name: "Right Meta",
                          width: 1,
                          color: "gray",
                      })
                    : new Key({
                          id: "ralt",
                          label: "ALT",
                          name: "Right Alt/Option",
                          width: 1,
                          color: "gray",
                      }),
                new Key({
                    id: "fn",
                    width: 1,
                    color: "gray",
                    name: "Function",
                    remappable: false,
                }),
                new Key({
                    id: "rctrl",
                    label: "CTRL",
                    name: "Right Control",
                    width: 1,
                    color: "gray",
                }),
                new Key({
                    id: "left",
                    label: "˂",
                    name: "Left Arrow",
                    width: 1,
                    color: "gray",
                }),
                new Key({
                    id: "down",
                    label: "˅",
                    name: "Down Arrow",
                    width: 1,
                    color: "gray",
                }),
                new Key({
                    id: "right",
                    label: "˃",
                    name: "Right Arrow",
                    width: 1,
                    color: "gray",
                }),
            ],
        ];
    },
};

export const Halo75 = {
    keycodes: halo75_keycodes,
    getLayout(mode) {
        let mac = mode === "mac";
        return [
            [
                new Key({
                    id: "esc",
                    label: "Esc",
                    color: "mint",
                    name: "Escape",
                }),
                new Key({
                    id: "f1",

                    altID: mac ? "fn_f1" : "brightnessdown",
                    altName: mac ? "F1 (Fn layer)" : "Brightness Down",
                    altDefaultMapping: mac ? "f1" : "brightnessdown",
                }),
                new Key({
                    id: "f2",

                    altID: mac ? "fn_f2" : "brightnessup",
                    altName: mac ? "F2 (Fn layer)" : "Brightness Up",
                    altDefaultMapping: mac ? "f2" : "brightnessup",
                }),
                new Key({
                    id: "f3",

                    altID: mac ? "fn_f3" : "missioncontrol",
                    altName: mac ? "F3 (Fn layer)" : "Mission Control",
                    altDefaultMapping: mac ? "f3" : "none",
                }),
                new Key({
                    id: "f4",

                    altID: mac ? "fn_f4" : "launchpad",
                    altName: mac ? "F4 (Fn layer)" : "Launchpad",
                    altDefaultMapping: mac ? "f4" : "none",
                }),
                new Key({
                    id: "f5",
                    color: "dark",
                    defaultMapping: mac ? "mic" : null,

                    altID: mac ? "fn_f5" : "mic",
                    altName: mac ? "F5 (Fn layer)" : "Mic",
                    altDefaultMapping: mac ? "f5" : "mic",
                }),
                new Key({
                    id: "f6",
                    color: "dark",
                    defaultMapping: mac ? "search" : null,

                    altID: mac ? "fn_f6" : "search",
                    altName: mac ? "F6 (Fn layer)" : "Search",
                    altDefaultMapping: mac ? "f6" : "search",
                }),
                new Key({
                    id: "f7",
                    color: "dark",

                    altID: mac ? "fn_f7" : "rewind",
                    altName: mac ? "F7 (Fn layer)" : "Rewind",
                    altDefaultMapping: mac ? "f7" : "rewind",
                }),
                new Key({
                    id: "f8",
                    color: "dark",

                    altID: mac ? "fn_f8" : "playpause",
                    altName: mac ? "F8 (Fn layer)" : "Play/Pause Toggle",
                    altDefaultMapping: mac ? "f8" : "playpause",
                }),
                new Key({
                    id: "f9",

                    altID: mac ? "fn_f9" : "forward",
                    altName: mac ? "F9 (Fn layer)" : "Fast-Forward",
                    altDefaultMapping: mac ? "f9" : "forward",
                }),
                new Key({
                    id: "f10",

                    altID: mac ? "fn_f10" : "mute",
                    altName: mac ? "F10 (Fn layer)" : "Mute",
                    altDefaultMapping: mac ? "f10" : "mute",
                }),
                new Key({
                    id: "f11",

                    altID: mac ? "fn_f11" : "volumedown",
                    altName: mac ? "F11 (Fn layer)" : "Volume Down",
                    altDefaultMapping: mac ? "f11" : "volumedown",
                }),
                new Key({
                    id: "f12",

                    altID: mac ? "fn_f12" : "volumeup",
                    altName: mac ? "F12 (Fn layer)" : "Volume Up",
                    altDefaultMapping: mac ? "f12" : "volumeup",
                }),
                new Key({
                    id: "screenshot",
                    color: "dark",
                    label: "✂️",
                    defaultMapping: mac ? "num4" : "sysrq",
                    defaultModifiers: mac ? ["meta", "shift"] : [],
                }),
                new Key({
                    id: "del",
                    label: "Del",
                    color: "dark",
                }),
                new Key({
                    id: "ins",
                    label: "Ins",
                    color: "dark",
                    name: "Insert",
                }),
            ],
            [
                new Key({
                    id: "grave",
                    label: "`",
                    color: "dark",
                    altLabel: "~",
                }),
                new Key({ id: "num1", label: "1", altLabel: "!" }),
                new Key({ id: "num2", label: "2", altLabel: "@" }),
                new Key({ id: "num3", label: "3", altLabel: "#" }),
                new Key({ id: "num4", label: "4", altLabel: "$" }),
                new Key({ id: "num5", label: "5", altLabel: "%" }),
                new Key({ id: "num6", label: "6", altLabel: "^" }),
                new Key({ id: "num7", label: "7", altLabel: "&" }),
                new Key({ id: "num8", label: "8", altLabel: "*" }),
                new Key({ id: "num9", label: "9", altLabel: "(" }),
                new Key({ id: "num0", label: "0", altLabel: ")" }),
                new Key({ id: "minus", label: "-", altLabel: "_" }),
                new Key({ id: "equal", label: "=", altLabel: "+" }),
                new Key({
                    id: "backspace",
                    label: "← Back",
                    width: 2.0,
                    color: "dark",
                }),
                new Key({ id: "home", label: "Home", color: "dark" }),
            ],
            [
                new Key({ id: "tab", label: "Tab", width: 1.5, color: "dark" }),
                new Key({ id: "q" }),
                new Key({ id: "w" }),
                new Key({ id: "e" }),
                new Key({ id: "r" }),
                new Key({ id: "t" }),
                new Key({ id: "y" }),
                new Key({ id: "u" }),
                new Key({ id: "i" }),
                new Key({ id: "o" }),
                new Key({ id: "p" }),
                new Key({
                    id: "rbracket",
                    label: "[",
                    name: "Left Bracket",
                    altLabel: "{",
                }),
                new Key({
                    id: "lbracket",
                    label: "]",
                    name: "Right Bracket",
                    altLabel: "}",
                }),
                new Key({
                    id: "backslash",
                    label: "\\",
                    width: 1.5,
                    name: "Backslash",
                    altLabel: "|",
                }),
                new Key({ id: "end", label: "End", color: "dark" }),
            ],
            [
                new Key({
                    id: "capslock",
                    label: "Caps",
                    name: "Caps Lock",
                    width: 1.75,
                    color: "dark",
                }),
                new Key({ id: "a" }),
                new Key({ id: "s" }),
                new Key({ id: "d" }),
                new Key({ id: "f" }),
                new Key({ id: "g" }),
                new Key({ id: "h" }),
                new Key({ id: "j" }),
                new Key({ id: "k" }),
                new Key({ id: "l" }),
                new Key({
                    id: "semicolon",
                    label: ";",
                    name: "Semicolon",
                    altLabel: ":",
                }),
                new Key({
                    id: "quote",
                    label: "'",
                    name: "Quote",
                    altLabel: '"',
                }),
                new Key({
                    id: "enter",
                    label: "↩ Enter",
                    width: 2.25,
                    color: "orange",
                }),
                new Key({
                    id: "pgup",
                    label: "PgUp",
                    color: "dark",
                    name: "Page Up",
                }),
            ],
            [
                new Key({
                    id: "lshift",
                    label: "⇧ Shift",
                    name: "Left Shift",
                    width: 2.25,
                    color: "dark",
                }),
                new Key({ id: "z" }),
                new Key({ id: "x" }),
                new Key({ id: "c" }),
                new Key({ id: "v" }),
                new Key({ id: "b" }),
                new Key({ id: "n" }),
                new Key({ id: "m" }),
                new Key({
                    id: "comma",
                    label: ",",
                    name: "Comma",
                    altLabel: "<",
                }),
                new Key({
                    id: "period",
                    label: ".",
                    name: "Period",
                    altLabel: ">",
                }),
                new Key({
                    id: "fwdslash",
                    label: "/",
                    name: "Slash",
                    altLabel: "?",
                }),
                new Key({
                    id: "rshift",
                    label: "⇧ Shift",
                    name: "Right Shift",
                    width: 1.75,
                    color: "dark",
                }),
                new Key({
                    id: "up",
                    label: "˄",
                    name: "Up Arrow",
                    color: "dark",
                }),
                new Key({
                    id: "pgdn",
                    label: "PgDn",
                    color: "dark",
                    name: "Page Down",
                }),
            ],
            [
                new Key({
                    id: "lctrl",
                    label: "Ctrl",
                    name: "Left Control",
                    width: 1.25,
                    color: "dark",
                }),
                mac
                    ? new Key({
                          id: "lalt",
                          label: "Opt",
                          name: "Left Alt/Option",
                          width: 1.25,
                          color: "dark",
                      })
                    : new Key({
                          id: "lmeta",
                          label: "Win",
                          name: "Left Meta",
                          width: 1.25,
                          color: "dark",
                      }),
                mac
                    ? new Key({
                          id: "lmeta",
                          label: "⌘",
                          name: "Left Meta",
                          width: 1.25,
                          color: "dark",
                      })
                    : new Key({
                          id: "lalt",
                          label: "Alt",
                          name: "Left Alt/Option",
                          width: 1.25,
                          color: "dark",
                      }),
                new Key({
                    id: "space",
                    label: "",
                    name: "Spacebar",
                    width: 6.25,
                    color: "yellow",
                }),
                mac
                    ? new Key({
                          id: "rmeta",
                          label: "⌘",
                          name: "Right Meta",
                          width: 1.25,
                          color: "dark",
                      })
                    : new Key({
                          id: "ralt",
                          label: "Alt",
                          name: "Right Alt/Option",
                          width: 1.25,
                          color: "dark",
                      }),
                new Key({
                    id: "fn",
                    label: "Fn",
                    width: 1.25,
                    color: "dark",
                    name: "Function",
                    remappable: false,
                }),
                new Key({
                    id: "__spacer",
                    width: 0.5,
                }),
                new Key({
                    id: "left",
                    label: "˂",
                    name: "Left Arrow",
                    width: 1,
                    color: "dark",
                }),
                new Key({
                    id: "down",
                    label: "˅",
                    name: "Down Arrow",
                    width: 1,
                    color: "dark",
                }),
                new Key({
                    id: "right",
                    label: "˃",
                    name: "Right Arrow",
                    width: 1,
                    color: "dark",
                }),
            ],
        ];
    },
};

export default {
    Air75,
    Halo75,
};
