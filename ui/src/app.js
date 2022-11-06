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
import { g, n } from "../lib/tinydom.js";

import "@fontsource/nunito/files/nunito-latin-500-normal.woff2";

import "./app.css";
import { Air75 } from "./keyboards.js";
import modifiers from "./modifiers.js";

class Remap {
    constructor() {
        this.remap = {};
    }
    get() {
        return this.remap;
    }
    set(incoming) {
        this.remap = incoming;
        redrawKeyboard();
        redrawOptions();
    }
}

window.keyboardFoundString = null;
window.currentKey = null;
window.remap = new Remap();

function writeYAML() {
    let remap = window.remap.get();
    window.ipc.sendRemap(remap);
}

function redrawKeyboard() {
    let container = g(".keyboard-container");
    let remap = window.remap.get();
    container.innerHTML = "";
    container.appendChild(n("div", e => {
        e.className = "keyboard card";
        let layout = window.layout;
        for (let currentRow in layout) {
            currentRow = Number(currentRow);
            let row = layout[currentRow];
            let currentColumn = 1;
            for (let key of row) {
                e.appendChild(n("div", e => {
                    e.id = key.id;
                    let width = key.width * 4;
                    let colorResolved = key.color;
                    let color = `var(--${colorResolved})`;
                    let label = key.label;
                    let labelColor = (colorResolved == "white") ? "var(--gray)"
                        : "var(--white)";
                    let className = "key";

                    if (remap[key.id] ?? false) {
                        className += " modified";
                    }

                    if (key.altLabel) {
                        className += " altlabel";
                    }
                    e.className = className;
                    e.style = `
                        background-color: ${color};
                        grid-column-start: ${currentColumn};
                        grid-column-end: ${currentColumn + width};
                        grid-row-start: ${currentRow + 1};
                        grid-row-end: ${currentRow + 1};
                    `;
                    e.onclick = onClickKey;
                    e.setAttribute("data", JSON.stringify(key));
                    currentColumn += width;
                    e.appendChild(n("p", e => {
                        let styleString = `text-shadow: 0 0 0 ${labelColor};`;
                        e.style = styleString;
                        e.innerHTML = label;
                        if (key.altLabel) {
                            e.innerHTML = `${key.altLabel} <br /> ${key.label}`;
                        }
                    }))
                }));
            }
        }
    }));
}

function redrawOptions() {
    let container = g(".option-container");
    let remap = window.remap.get();
    container.innerHTML = "";
    container.appendChild(n("div", e => {
        e.className = "option-matrix card";
        let key = window.currentKey;
        let modifierCount = 1;
        if (key) {
            if (!key.remappable) {
                e.appendChild(n("p", e => {
                    e.innerHTML = `The ${key.name} key cannot be remapped.`
                }))
                return
            }

            let currentRemap = remap[key.id] ?? {};
            let currentModifiers = currentRemap.modifiers ?? key.defaultModifiers;
            for (let id in modifiers) {
                let modifier = modifiers[id];
                e.appendChild(n("div", e => {
                    if (currentModifiers.indexOf(id) !== -1) {
                        e.className = "key selected";
                    } else {
                        e.className = "key";
                    }
                    e.id = `modifier-${id}`;
                    e.style = `
                        grid-column-start: ${modifierCount};
                        grid-column-end: ${modifierCount};
                        grid-row-start: 2;
                        grid-row-end: 2;
    
                    `;
                    e.appendChild(n("p", e => {
                        e.innerHTML = modifier.label;
                    }))
                    e.onclick = onClickModifier;
                }));
                modifierCount += 1;
            }
            e.appendChild(n("select", e => {
                e.id = `keycode-selector`;
                e.style = `
                    grid-column-start: ${modifierCount};
                    grid-column-end: ${modifierCount + 3};
                    grid-row-start: 2;
                    grid-row-end: 2;
                `;
                for (let keycode in Air75.keycodes) {
                    e.appendChild(n("option", e => {
                        e.innerHTML = keycode;
                        if (keycode == currentRemap.key) {
                            e.selected = true;
                        } else if (currentRemap.key == null && keycode == key.defaultMapping) {
                            e.selected = true;
                        }
                    }));
                }
                e.onchange = updateKeymap;
            }));

            e.appendChild(n("h3", e => {
                e.style = `
                    grid-column-start: ${1};
                    grid-column-end: ${modifierCount + 3};
                    grid-row-start: 1;
                    grid-row-end: 1;
                `;
                e.innerHTML = `Current Key: ${key.name}`;
            }))
            modifierCount += 3;
        } else {
            e.appendChild(n("h3", e => {
                e.style = `
                    grid-column-start: 1;
                    grid-column-end: ${modifierCount + 3};
                    grid-row-start: 1;
                    grid-row-end: 1;
                `;
                e.id = "no-key-selected";
                e.innerHTML = "No key selected.";
            }));
            modifierCount += 3;
        }


        e.appendChild(n("p", e => {
            e.style = `
                grid-column-start: ${modifierCount + 2};
                grid-column-end: ${modifierCount + 6};
                grid-row-start: 1;
                grid-row-end: 1;
            `;
            e.id = "keyboard-field";
            e.innerHTML = window.keyboardFoundString ?? "No keyboard found.<br />File > Reload Keyboard to retry.";
        }));

        e.appendChild(n("div", e => {
            e.className = `key`;
            if (window.keyboardFoundString) {
                e.className = `key active`;
            }
            e.id = `write-key`;
            e.style = `
                grid-column-start: ${modifierCount + 2};
                grid-column-end: ${modifierCount + 6};
                grid-row-start: 2;
                grid-row-end: 2;
            `;
            e.appendChild(n("p", e => {
                e.innerHTML = "WRITE";
            }));
            e.onclick = writeYAML;
        }));

    }))
}

function setsEqual(lhs, rhs) {
    const _difference = new Set(lhs);
    for (const elem of rhs) {
        if (_difference.has(elem)) {
            _difference.delete(elem);
        } else {
            _difference.add(elem);
        }
    }
    return _difference.size == 0;
}

/**
 * 
 * @param {Event} event 
 */
function updateKeymap(event) {
    let remap = window.remap.get();
    let key = window.currentKey;

    let currentRemap = remap[key.id] ?? {};

    let incomingID = g("#keycode-selector").value;
    let incomingModifiers = new Set();
    let defaultModifiers = new Set(key.defaultModifiers);
    for (let modifier in modifiers) {
        let modifierNode = g(`#modifier-${modifier}`);
        let selected = modifierNode.className.includes("selected");
        if (selected) {
            incomingModifiers.add(modifier);
        }
    }

    let modifiersEqual = setsEqual(incomingModifiers, defaultModifiers);

    if (incomingID == key.defaultMapping && modifiersEqual) {
        remap[key.id] = {};
        delete remap[key.id];
    } else {
        currentRemap.key = incomingID;
        currentRemap.modifiers = [...incomingModifiers];
        if (modifiersEqual) {
            delete currentRemap.modifiers;
        }
        remap[key.id] = currentRemap;
    }

    window.remap.set(remap);
}

/**
 * 
 * @param {MouseEvent} event 
 */
function onClickModifier(event) {
    let target = event.target;
    if (target.className.includes("selected")) {
        target.className = "key";
    } else {
        target.className = "key selected";
    }

    updateKeymap();
}

/**
 * 
 * @param {MouseEvent} event 
 */
function onClickKey(event) {
    window.currentKey = JSON.parse(event.target.getAttribute("data"));
    redrawOptions();
}


async function main() {
    window.layout = Air75.getLayout("win");

    let app = g(".app");

    app.appendChild(n("div", e => {
        e.style = "padding-top: 10px;"
        e.appendChild(n("h1", e => {
            e.innerHTML = "νδ";
        }))
    }));

    app.appendChild(n("p", e => {
        e.appendChild(n("div", e => {
            e.className = "keyboard-container";
        }));
    }));

    redrawKeyboard();

    app.appendChild(n("p", e => {
        e.appendChild(n("div", e => {
            e.className = "option-container";
        }));
    }));

    redrawOptions();

    window.ipc.onGetKeyboardInfo((_, { info }) => {
        window.keyboardFoundString = info;
        redrawOptions();
    });
    window.ipc.getKeyboardInfo();

    window.ipc.onLoadKeymap((_, { keymap }) => {
        let keys = keymap.keys ?? {};
        for (let key in keys) {
            let remap = keys[key];
            if (typeof remap == "string") {
                keys[key] = { key: remap };
            }
        }
        window.remap.set(keys);
    });

    window.ipc.onSaveKeymap((e, { filePath }) => {
        e.sender.send("save-keymap-reply", { remap: window.remap.get(), filePath })
    });

}

main();
