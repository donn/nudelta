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

window.currentKey = "esc";
window.remap = new Remap();

function redrawKeyboard() {
    let container = g(".keyboard-container");
    let remap = window.remap.get();
    container.innerHTML = "";
    container.appendChild(n("div", e => {
        e.className = "keyboard";
        for (let currentRow in Air75.layout) {
            currentRow = Number(currentRow);
            let row = Air75.layout[currentRow];
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
                    if (remap[key.id] ?? false) {
                        e.className = "key modified"
                    } else {
                        e.className = "key";
                    }
                    e.style = `
                        background-color: ${color};
                        grid-column-start: ${currentColumn};
                        grid-column-end: ${currentColumn + width};
                        grid-row-start: ${currentRow + 1};
                        grid-row-end: ${currentRow + 1};
                    `;
                    e.onclick = onClickKey;
                    currentColumn += width;
                    e.appendChild(n("p", e => {
                        let styleString = `text-shadow: 0 0 0 ${labelColor};`;
                        e.style = styleString;
                        e.innerHTML = label;
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
        e.className = "option-matrix";
        let key = Air75.byID[currentKey];
        if (!key.remappable) {
            e.appendChild(n("p", e => {
                e.innerHTML = `The ${key.name} key cannot be remapped.`
            }))
            return
        }

        let currentRemap = remap[window.currentKey] ?? {};
        let currentModifiers = currentRemap.modifiers ?? [];
        let modifierCount = 1;
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
                    grid-column-end: ${modifierCount + 1};
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
            for (let key in Air75.keycodes) {
                e.appendChild(n("option", e => {
                    e.innerHTML = key;
                    if (key == currentRemap.key) {
                        e.selected = true;
                    } else if (currentRemap.key == null && key == window.currentKey) {
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
    }))
}
/**
 * 
 * @param {Event} event 
 */
function updateKeymap(event) {
    let remap = window.remap.get();
    let currentRemap = remap[window.currentKey] ?? {};

    let incomingID = g("#keycode-selector").value;
    let incomingModifiers = [];
    for (let modifier in modifiers) {
        let modifierNode = g(`#modifier-${modifier}`);
        let selected = modifierNode.className.includes("selected");
        if (selected) {
            incomingModifiers.push(modifier);
        }
    }

    if (incomingID == window.currentKey && incomingModifiers.length == 0) {
        remap[window.currentKey] = {};
        delete remap[window.currentKey];
    } else {
        currentRemap.key = incomingID;
        currentRemap.modifiers = incomingModifiers;
        if (currentRemap.modifier.length == 0) {
            delete currentRemap.modifiers;
        }
        remap[window.currentKey] = currentRemap;
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
    window.currentKey = event.target.id;
    redrawOptions();
}


async function main() {
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

    app.appendChild(n("div", e => {
        e.className = "option-container";
    }));
}

main();
