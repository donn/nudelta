const { ipcRenderer } = require("electron");

const macOS = process.platform == "darwin";

function tweaks() {
}

window.addEventListener('DOMContentLoaded', tweaks);

ipcRenderer.on("load-keymap", (ev, { keymap }) => {
    console.log(keymap);
    let keys = keymap.keys ?? {};
    let rawFound = false;
    for (let key in keys) {
        let remap = keys[key];
        if (typeof remap == "string") {
            keys[key] = { key: remap };
        }
        if (key.raw != null) {
            rawFound = true;
            delete key.raw;
        }
    }
    window.remap.set(keys);
});

ipcRenderer.on("save-keymap", (ev, { filePath }) => {
    ipcRenderer.send("save-keymap-callback", { remap: window.remap.get(), filePath })
});