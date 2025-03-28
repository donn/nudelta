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
"use strict";
import { g, n } from "../lib/tinydom.js";

import "@fontsource/nunito/files/nunito-latin-500-normal.woff2";

import "./app.css";
import "./toggle.css";
import keyboards from "./keyboards.js";
import modifiers from "./modifiers.js";

class Config {
    constructor() {
        this.winRemap = {};
        this.macRemap = {};
    }
    getRemap(mode) {
        if (mode === "mac") {
            return Object.assign({}, this.macRemap);
        } else {
            return Object.assign({}, this.winRemap);
        }
    }

    setRemap(mode, incoming) {
        if (mode === "mac") {
            this.macRemap = incoming;
        } else {
            this.winRemap = incoming;
        }
        redrawKeyboard();
        redrawOptions();
    }

    unmarshall(config) {
        let keys = config.keys ?? {};
        for (let key in keys) {
            let remap = keys[key];
            if (typeof remap == "string") {
                keys[key] = { key: remap };
            }
        }

        let mackeys = config.mackeys ?? {};
        for (let key in keys) {
            let remap = mackeys[key];
            if (typeof remap == "string") {
                mackeys[key] = { key: remap };
            }
        }
        this.winRemap = keys;
        this.setRemap("mac", mackeys);
    }

    marshall() {
        return {
            keys: this.winRemap,
            mackeys: this.macRemap,
        };
    }
}

window.unsafeRemapping = false;
function safetyOff(silent = false) {
    if (!silent) {
        alert(
            "Enabling unsafe remapping.\nIf you remap Fn, this will change even your factory reset shortcut, so be careful!"
        );
    }
    window.unsafe = true;
    redrawOptions();
}
window.mode = "win";
window.keyboardInfo = null;
window.lastKey = null;
window.currentKey = null;
window.clickCount = 0;
window.config = new Config();

function writeYAML() {
    let marshalled = window.config.marshall();
    console.log("marshalled: ", marshalled);
    window.ipc.sendConfig(marshalled);
}

function redrawKeyboard() {
    if (window.keyboardInfo === null) {
        return;
    }
    let keyboard = keyboards[window.keyboardInfo.kind];
    let container = g(".keyboard-container");
    let remap = window.config.getRemap(window.mode);
    container.innerHTML = "";
    container.appendChild(
        n("div", (e) => {
            e.className = "keyboard card";
            let layout = keyboard.getLayout(window.mode);
            for (let currentRow in layout) {
                currentRow = Number(currentRow);
                let row = layout[currentRow];
                let currentColumn = 1;
                for (let key of row) {
                    e.appendChild(
                        n("div", (e) => {
                            e.id = key.id;
                            let width = key.width * 4;
                            let height = key.height * 4;
                            let colorResolved = key.color;
                            let color = `var(--${colorResolved})`;
                            let label = key.label;
                            let labelColor =
                                colorResolved == "white"
                                    ? "var(--gray)"
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
                                grid-row-end: ${height > 4 ? "span 2" : currentRow + 1};
                                ${height > 4 ? "height: 115px;": ""};
                                ${e.id == "__spacer" ? "display: none;" : ""}
                            `;
                            e.onclick = onClickKey;
                            e.setAttribute("data", JSON.stringify(key));
                            currentColumn += width;
                            e.appendChild(
                                n("span", (e) => {
                                    let styleString = `text-shadow: 0 0 0 ${labelColor};`;
                                    e.style = styleString;
                                    e.innerHTML = label;
                                    if (key.altLabel) {
                                        e.innerHTML = `${key.altLabel} <br /> ${key.label}`;
                                    }
                                })
                            );
                        })
                    );
                }
            }
        })
    );

    let switcherContainer = g(".switcher");
    switcherContainer.innerHTML = "";
    switcherContainer.appendChild(
        n("label", (e) => {
            e.className = "toggle-switchy";
            e.setAttribute("data-style", "rounded");
            e.setAttribute("for", "mode-switcher");
            e.appendChild(
                n("input", (e) => {
                    e.id = "mode-switcher";
                    e.setAttribute("type", "checkbox");
                    e.checked = window.mode == "mac";
                    e.onchange = (ev) => {
                        window.mode = ev.target.checked ? "mac" : "win";
                        window.currentKey = null;
                        console.log(window.mode);
                        redrawKeyboard();
                        redrawOptions();
                    };
                })
            );
            e.appendChild(
                n("span", (e) => {
                    e.className = "toggle";
                    e.appendChild(
                        n("span", (e) => {
                            e.className = "switch";
                        })
                    );
                })
            );
        })
    );
}

function drawOptionArray(e, remap, key, alt, column, row) {
    let id = alt ? key.altID : key.id;
    let defaultMapping = alt ? key.altDefaultMapping : key.defaultMapping;
    let defaultModifiers = alt ? key.altDefaultModifiers : key.defaultModifiers;

    let currentRemap = {};
    let currentModifiers = defaultModifiers;
    if (remap[id]) {
        currentRemap = remap[id];
        currentModifiers = currentRemap.modifiers ?? [];
    }

    let keycodes = window.keyboardInfo
        ? keyboards[window.keyboardInfo.kind].keycodes
        : [];

    for (let modifierID in modifiers) {
        let modifier = modifiers[modifierID];
        e.appendChild(
            n("div", (e) => {
                if (currentModifiers.indexOf(modifierID) !== -1) {
                    e.className = "key selected";
                } else {
                    e.className = "key";
                }
                let elementID = `modifier-${modifierID}`;
                if (alt) {
                    elementID += "-alt";
                }
                e.id = elementID;
                e.style = `
                    grid-column-start: ${column};
                    grid-column-end: ${column};
                    grid-row-start: ${row};
                    grid-row-end: ${row + 1};
                `;
                e.appendChild(
                    n("span", (e) => {
                        e.innerHTML = modifier.label;
                    })
                );
                e.onclick = onClickModifier;
            })
        );
        column += 1;
    }
    e.appendChild(
        n("select", (e) => {
            let elementID = "keycode-selector";
            if (alt) {
                elementID += "-alt";
            }
            e.id = elementID;
            e.style = `
                grid-column-start: ${column};
                grid-column-end: ${column + 2};
                grid-row-start: ${row};
                grid-row-end: ${row + 1};
            `;
            column += 2;
            for (let keycode in keycodes) {
                e.appendChild(
                    n("option", (e) => {
                        e.innerHTML = keycode;
                        if (keycode == currentRemap.key) {
                            e.selected = true;
                        } else if (
                            currentRemap.key == null &&
                            keycode == defaultMapping
                        ) {
                            e.selected = true;
                        }
                    })
                );
            }
            e.onchange = updateKeymap;
        })
    );

    return column;
}

function redrawOptions() {
    let container = g(".option-container");
    container.innerHTML = "";

    if (window.keyboardInfo === null) {
        container.appendChild(
            n("div", (e) => {
                e.style = `
                grid-column-start: 1;
                grid-column-end: 1;
                grid-row-start: 1;
                grid-row-end: 1;
            `;
                e.className = "keyboard-field";
                e.appendChild(
                    n("p", (e) => {
                        e.innerHTML =
                            "No supported keyboard found.<br />Make sure it's plugged in via USB, then File > Reload Keyboard to retry.";
                    })
                );
            })
        );
        return;
    }

    let remap = window.config.getRemap(window.mode);
    container.appendChild(
        n("div", (e) => {
            e.className = "option-matrix card";
            let key = window.currentKey;
            let columnCount = 1;
            let remappable = (key && key.remappable) || window.unsafe;
            if (key && remappable) {
                let altIDExists = !!key.altID;

                columnCount = drawOptionArray(
                    e,
                    remap,
                    key,
                    false,
                    1,
                    altIDExists ? 2 : 3
                );
                e.appendChild(
                    n("h3", (e) => {
                        e.style = `
                            grid-column-start: 1;
                            grid-column-end: ${columnCount};
                            grid-row-start: ${altIDExists ? 1 : 2};
                            grid-row-end: ${altIDExists ? 2 : 3};
                        `;
                        e.innerHTML = `${key.name}`;
                    })
                );
                if (altIDExists) {
                    columnCount = drawOptionArray(e, remap, key, true, 1, 4);
                    e.appendChild(
                        n("h3", (e) => {
                            e.style = `
                                grid-column-start: 1;
                                grid-column-end: ${columnCount};
                                grid-row-start: 3;
                                grid-row-end: 4;
                            `;
                            e.innerHTML = `${key.altName}`;
                        })
                    );
                } else {
                    e.appendChild(
                        n("h3", (e) => {
                            e.style = `
                                grid-column-start: 1;
                                grid-column-end: ${columnCount};
                                grid-row-start: 4;
                                grid-row-end: 5;
                            `;
                        })
                    );
                }
            } else if (key && !remappable) {
                e.appendChild(
                    n("h3", (e) => {
                        e.style = `
                            grid-column-start: 1;
                            grid-column-end: ${columnCount + 6};
                            grid-row-start: 2;
                            grid-row-end: 5;
                        `;
                        e.innerHTML = `The ${key.name} key cannot be remapped.`;
                    })
                );
                columnCount += 6;
            } else {
                e.appendChild(
                    n("h3", (e) => {
                        e.style = `
                            grid-column-start: 1;
                            grid-column-end: ${columnCount + 6};
                            grid-row-start: 2;
                            grid-row-end: 5;
                        `;
                        e.id = "no-key-selected";
                        e.innerHTML = "No key selected.";
                    })
                );
                columnCount += 6;
            }

            columnCount += 1;

            e.appendChild(
                n("div", (e) => {
                    e.style = `
                        grid-column-start: ${columnCount};
                        grid-column-end: ${columnCount + 3};
                        grid-row-start: 2;
                        grid-row-end: 3;
                    `;
                    e.className = "keyboard-field";
                    e.appendChild(
                        n("p", (e) => {
                            e.innerHTML = window.keyboardInfo.info;
                        })
                    );
                })
            );

            e.appendChild(
                n("div", (e) => {
                    e.className = `key write-key`;
                    if (window.keyboardInfo !== null) {
                        e.className = `key write-key active`;
                    }
                    e.style = `
                        grid-column-start: ${columnCount};
                        grid-column-end: ${columnCount + 3};
                        grid-row-start: 3;
                        grid-row-end: 4;
                    `;
                    e.appendChild(
                        n("p", (e) => {
                            e.innerHTML = "WRITE";
                        })
                    );
                    e.onclick = writeYAML;
                })
            );
        })
    );
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
    let remap = window.config.getRemap(window.mode);
    console.log("Before: ", JSON.stringify(remap));
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

    if (key.altID) {
        let currentRemap = remap[key.altID] ?? {};
        let incomingID = g("#keycode-selector-alt").value;
        let incomingModifiers = new Set();
        let defaultModifiers = new Set(key.altDefaultModifiers);
        for (let modifier in modifiers) {
            let modifierNode = g(`#modifier-${modifier}-alt`);
            let selected = modifierNode.className.includes("selected");
            if (selected) {
                incomingModifiers.add(modifier);
            }
        }

        let modifiersEqual = setsEqual(incomingModifiers, defaultModifiers);

        if (incomingID == key.altDefaultMapping && modifiersEqual) {
            remap[key.altID] = {};
            delete remap[key.altID];
        } else {
            currentRemap.key = incomingID;
            currentRemap.modifiers = [...incomingModifiers];
            if (modifiersEqual) {
                delete currentRemap.modifiers;
            }
            remap[key.altID] = currentRemap;
        }
    }
    console.log("After: ", JSON.stringify(remap));
    window.config.setRemap(window.mode, remap);
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
    window.lastKey = window.currentKey;
    window.currentKey = JSON.parse(event.target.getAttribute("data"));
    if (window.lastKey && window.lastKey.id == window.currentKey.id) {
        window.clickCount += 1;
    } else {
        window.clickCount = 1;
    }
    if (!window.currentKey.remappable) {
        let remap = window.config.getRemap(window.mode);

        if (remap[window.currentKey.id]) {
            safetyOff(true);
        } else if (window.clickCount == 5) {
            safetyOff();
        }
    }
    redrawOptions();
}

async function main() {
    let app = g(".app");

    app.appendChild(
        n("div", (e) => {
            e.style = "padding-top: 10px;";
            e.appendChild(
                n("h1", (e) => {
                    e.innerHTML = "νδ";
                })
            );
        })
    );

    app.appendChild(
        n("p", (e) => {
            e.appendChild(
                n("div", (e) => {
                    e.className = "keyboard-container";
                })
            );
        })
    );

    app.appendChild(
        n("p", (e) => {
            e.className = "switcher";
        })
    );

    redrawKeyboard();

    app.appendChild(
        n("p", (e) => {
            e.appendChild(
                n("div", (e) => {
                    e.className = "option-container";
                })
            );
        })
    );

    redrawOptions();

    window.ipc.onGetVersion((_, { version }) => {
        console.log(version);
    });
    window.ipc.getVersion();

    window.ipc.onGetKeyboardInfo((_, { info }) => {
        window.keyboardInfo = info;
        console.log(info);
        redrawKeyboard();
        redrawOptions();
    });
    window.ipc.getKeyboardInfo();

    window.ipc.onLoadConfig((_, { config }) => {
        window.config.unmarshall(config);
    });

    window.ipc.onSaveConfig((e, { filePath }) => {
        e.sender.send("save-config-reply", {
            config: window.config.marshall(),
            filePath,
        });
    });
}

main();
