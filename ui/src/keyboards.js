import air75_keycodes from "../../res/air75_keycodes.yml"

class Key {
    constructor({ id, color, label, width, name, remappable }) {
        console.log(id);
        this.id = id;
        this.color = color ?? "white";
        this.label = label ?? this.id.toUpperCase();
        this.width = width ?? 1.0;
        this.name = name ?? this.id.charAt(0).toUpperCase() + this.id.slice(1);
        this.remappable = remappable ?? true;
    }
}

export const Air75 = {
    layout: [
        [
            new Key({ id: "esc", color: "mint", name: "Escape" }),
            new Key({ id: "f1" }),
            new Key({ id: "f2" }),
            new Key({ id: "f3" }),
            new Key({ id: "f4" }),
            new Key({ id: "f5", color: "gray" }),
            new Key({ id: "f6", color: "gray" }),
            new Key({ id: "f7", color: "gray" }),
            new Key({ id: "f8", color: "gray" }),
            new Key({ id: "f9" }),
            new Key({ id: "f10" }),
            new Key({ id: "f11" }),
            new Key({ id: "f12" }),
            new Key({ id: "screenshot", color: "gray", label: "✂️" }),
            new Key({ id: "assistant", color: "gray", label: "🐱" }),
            new Key({ id: "del", color: "gray", name: "Delete" })
        ],
        [
            new Key({ id: "grave", label: "`" }),
            new Key({ id: "num1", label: "1" }),
            new Key({ id: "num2", label: "2" }),
            new Key({ id: "num3", label: "3" }),
            new Key({ id: "num4", label: "4" }),
            new Key({ id: "num5", label: "5" }),
            new Key({ id: "num6", label: "6" }),
            new Key({ id: "num7", label: "7" }),
            new Key({ id: "num8", label: "8" }),
            new Key({ id: "num9", label: "9" }),
            new Key({ id: "num0", label: "0" }),
            new Key({ id: "minus", label: "-" }),
            new Key({ id: "equal", label: "=" }),
            new Key({ id: "backspace", width: 2.0, color: "gray" }),
            new Key({ id: "pgup", color: "gray", name: "Page Up" })
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
            new Key({ id: "rbracket", label: "[", name: "Left Bracket" }),
            new Key({ id: "lbracket", label: "]", name: "Right Bracket" }),
            new Key({
                id: "backslash", label: "\\", width: 1.5, name: "Backslash"
            }),
            new Key({ id: "pgdn", color: "gray", name: "Page Down" })
        ],
        [
            new Key({ id: "capslock", label: "CAPS", name: "Caps Lock", width: 1.75, color: "gray" }),
            new Key({ id: "a" }),
            new Key({ id: "s" }),
            new Key({ id: "d" }),
            new Key({ id: "f" }),
            new Key({ id: "g" }),
            new Key({ id: "h" }),
            new Key({ id: "j" }),
            new Key({ id: "k" }),
            new Key({ id: "l" }),
            new Key({ id: "semicolon", label: ";", name: "Semicolon" }),
            new Key({ id: "quote", label: "'", name: "Quote" }),
            new Key({ id: "enter", width: 2.25, color: "orange" }),
            new Key({ id: "home", color: "gray" })
        ],
        [
            new Key({ id: "lshift", label: "SHIFT", name: "Left Shift", width: 2.25, color: "gray" }),
            new Key({ id: "z" }),
            new Key({ id: "x" }),
            new Key({ id: "c" }),
            new Key({ id: "v" }),
            new Key({ id: "b" }),
            new Key({ id: "n" }),
            new Key({ id: "m" }),
            new Key({ id: "comma", label: ",", name: "Comma" }),
            new Key({ id: "period", label: ".", name: "Period" }),
            new Key({ id: "fwdslash", label: "/", name: "Slash" }),
            new Key({ id: "rshift", label: "SHIFT", name: "Right Shift", width: 1.75, color: "gray" }),
            new Key({ id: "up", label: "˄", name: "Up Arrow", color: "gray" }),
            new Key({ id: "end", color: "gray" })
        ],
        [
            new Key({ id: "lctrl", label: "CTRL", name: "Left Control", width: 1.25, color: "gray" }),
            new Key({ id: "lmeta", label: "WIN", name: "Left Meta", width: 1.25, color: "gray" }),
            new Key({ id: "lalt", label: "ALT", name: "Left Alt/Option", width: 1.25, color: "gray" }),
            new Key({ id: "space", label: "", name: "Spacebar", width: 6.25, color: "yellow" }),
            new Key({ id: "ralt", label: "ALT", name: "Right Alt/Option", width: 1, color: "gray" }),
            new Key({ id: "fn", width: 1, color: "gray", name: "Function", remappable: false }),
            new Key({ id: "rctrl", label: "CTRL", name: "Right Control", width: 1, color: "gray" }),
            new Key({ id: "left", label: "˂", name: "Left Arrow", width: 1, color: "gray" }),
            new Key({ id: "down", label: "˅", name: "Down Arrow", width: 1, color: "gray" }),
            new Key({ id: "right", label: "˃", name: "Right Arrow", width: 1, color: "gray" }),

        ]
    ]
};

Air75.byID = {};
for (let row of Air75.layout) {
    for (let key of row) {
        Air75.byID[key.id] = key;
    }
}

Air75.keycodes = air75_keycodes;