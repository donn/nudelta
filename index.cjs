// Modules to control application life and create native browser window
const electron = require("electron");
const { app, dialog, BrowserWindow, Menu, ipcMain, shell } = electron;

const macOS = process.platform == "darwin";

const path = require("path");
const fs = require("fs-extra");
const YAML = require("yaml");

const libnd = require("./node-libnd.node");

function createWindow() {
    // Create the browser window.
    const mainWindow = new BrowserWindow({
        width: 1100,
        height: 900,
        webPreferences: {
            preload: path.join(__dirname, "ui", "src", "preload.js"),
        },
        icon: path.join(__dirname, "res", "nudelta.icns"),
    });

    // and load the index.html of the app.
    mainWindow.loadFile(path.join(__dirname, "ui", "index.html"));

    const menu = Menu.buildFromTemplate(
        (macOS ? [{ role: "appMenu" }] : []).concat([
            {
                label: "File",
                submenu: [
                    {
                        type: "normal",
                        label: "Open",
                        accelerator: "CommandOrControl+O",
                        click: async () => {
                            let { filePaths } = await dialog.showOpenDialog(
                                mainWindow,
                                {
                                    title: "Open Remap File…",
                                    buttonLabel: "Open",
                                    filters: [
                                        {
                                            name: "Nudelta Config File",
                                            extensions: ["yml", "yaml"],
                                        },
                                    ],
                                }
                            );

                            if (filePaths.length == 0) {
                                return;
                            }

                            let file = filePaths[0];

                            mainWindow.currentFile = file;

                            let value = fs.readFileSync(
                                mainWindow.currentFile,
                                "utf8"
                            );

                            try {
                                libnd.validateYAML(value);

                                let config = YAML.parse(value);

                                mainWindow.webContents.send("load-config", {
                                    config,
                                });
                            } catch (err) {
                                await dialog
                                    .showErrorBox(
                                        "Invalid YAML file",
                                        err.message
                                    )
                                    .catch((err) => {
                                        console.error(err);
                                    });
                            }
                        },
                    },
                    {
                        type: "normal",
                        label: "Save",
                        accelerator: "CommandOrControl+S",
                        click: async () => {
                            let { filePath } = await dialog.showSaveDialog(
                                mainWindow,
                                {
                                    title: "Save Remap File…",
                                    buttonLabel: "Save",
                                    filters: [
                                        {
                                            name: "Nudelta Config File",
                                            extensions: ["yml", "yaml"],
                                        },
                                    ],
                                    default: mainWindow.currentFile,
                                }
                            );

                            mainWindow.webContents.send("save-config", {
                                filePath,
                            });
                        },
                    },
                    {
                        type: "normal",
                        label: "Reload Keyboard",
                        accelerator: "CommandOrControl+R",
                        click: async () => {
                            sendKeyboardInfo(mainWindow);
                        },
                    },
                    macOS ? { role: "close" } : { role: "quit" },
                ],
            },
            { role: "editMenu" },
            { role: "windowMenu" },
            {
                label: "View",
                submenu: [
                    { type: "separator" },
                    { role: "forceReload" },
                    { role: "toggleDevTools" },
                    { type: "separator" },
                    { role: "resetZoom" },
                    { role: "zoomIn" },
                    { role: "zoomOut" },
                    { type: "separator" },
                    { role: "togglefullscreen" },
                ],
            },
            {
                role: "help",
                submenu: [
                    {
                        label: "Report A Bug",
                        click: async () => {
                            await shell.openExternal(
                                "https://github.com/donn/nudelta/issues"
                            );
                        },
                    },
                    {
                        label: "Code Repository",
                        click: async () => {
                            await shell.openExternal(
                                "https://github.com/donn/nudelta"
                            );
                        },
                    },
                    {
                        label: "Open Source Acknowledgements",
                        click: async () => {
                            await shell.openExternal(
                                "https://raw.githubusercontent.com/donn/nudelta/main/OSAcknowledgements.txt"
                            );
                        },
                    },
                ],
            },
        ])
    );
    Menu.setApplicationMenu(menu);
}

app.whenReady().then(() => {
    createWindow();

    app.on("activate", function () {
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    });
});

app.on("window-all-closed", function () {
    if (!macOS) app.quit();
});

ipcMain.on("save-config-reply", async (_, { config, filePath }) => {
    let string = YAML.stringify(config);
    await fs.writeFile(filePath, string);
});

async function sendKeyboardInfo(sender) {
    try {
        let info = libnd.getKeyboardInfo();
        sender.send("get-keyboard-info-reply", { info });
    } catch (err) {
        let message = err.message;
        if (err.kind === "Permissions Error") {
            let commands = message.split("\n\n")[1];
            if (commands ?? false) {
                clipboard.writeText(commands);
                message =
                    "Unable to read HID devices. Please run the commands copied to your clipboard in a terminal window then restart Nudelta.";
            }
        }
        dialog.showErrorBox(err.kind, message);
    }
}

ipcMain.on("get-keyboard-info", (ev) => sendKeyboardInfo(ev.sender));

ipcMain.on("write-yaml", async (ev, config) => {
    let serialized = YAML.stringify(config);
    console.log(`Writing ${serialized}...`);
    try {
        libnd.setKeymapFromYAML(serialized);
    } catch (err) {
        dialog.showErrorBox("Failed to write config", err.message);
        await sendKeyboardInfo(ev.sender);
        return;
    }
    await dialog.showMessageBox(BrowserWindow.getFocusedWindow(), {
        message: "Wrote configuration successfully!",
    });
});

ipcMain.on("get-version", async (ev) => {
    ev.sender.send("get-version-reply", { version: app.getVersion() });
});
