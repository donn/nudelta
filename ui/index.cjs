// Modules to control application life and create native browser window
const electron = require("electron");
const { app, dialog, BrowserWindow, Menu, ipcMain } = electron;

const macOS = process.platform == "darwin";

const path = require("path");
const fs = require("fs-extra");
const YAML = require("yaml");

function createWindow() {
    // Create the browser window.
    const mainWindow = new BrowserWindow({
        width: 1240,
        height: 720,
        webPreferences: {
            preload: path.join(__dirname, 'src', 'preload.js'),
            contextIsolation: false
        }
    });

    // and load the index.html of the app.
    mainWindow.loadFile(path.join(__dirname, 'index.html'))

    const menu = Menu.buildFromTemplate([
        { role: 'appMenu' },
        {
            label: 'File',
            submenu: [
                {
                    type: "normal",
                    label: 'Open',
                    accelerator: "CommandOrControl+O",
                    click: async () => {
                        let { filePaths } = await dialog.showOpenDialog(mainWindow, {
                            title: "Open Remap File…",
                            buttonLabel: "Open",
                            filters: [
                                { name: "Nudelta Remap File", extensions: ["yml", "yaml"] }
                            ]
                        });

                        if (filePaths.length == 0) {
                            return;
                        }

                        let file = filePaths[0];

                        mainWindow.currentFile = file;

                        let value = fs.readFileSync(mainWindow.currentFile, "utf8");

                        let keymap = YAML.parse(value);

                        mainWindow.webContents.send("load-keymap",
                            { keymap }
                        );
                    }
                },
                {
                    type: "normal",
                    label: 'Save',
                    accelerator: "CommandOrControl+S",
                    click: async () => {
                        let { filePath, } = await dialog.showSaveDialog(mainWindow, {
                            title: "Save Remap File…",
                            buttonLabel: "Save",
                            filters: [
                                { name: "Nudelta Remap File", extensions: ["yml", "yaml"] }
                            ],
                            default: mainWindow.currentFile
                        });

                        mainWindow.webContents.send("save-keymap", { filePath });
                    }
                },
                macOS ? { role: 'close' } : { role: 'quit' }
            ]
        },
        { role: 'editMenu' },
        { role: 'windowMenu' },
        {
            label: 'View',
            submenu: [
                { type: 'separator' },
                { role: 'forceReload' },
                { role: 'toggleDevTools' },
                { type: 'separator' },
                { role: 'resetZoom' },
                { role: 'zoomIn' },
                { role: 'zoomOut' },
                { type: 'separator' },
                { role: 'togglefullscreen' }
            ]
        },
        {
            role: 'help',
            submenu: [
                {
                    label: 'Repo',
                    click: async () => {
                        const { shell } = electron;
                        await shell.openExternal('https://github.com/donn/nudelta')
                    }
                }
            ]
        }
    ]);
    Menu.setApplicationMenu(menu);

    // Open the DevTools.
    // mainWindow.webContents.openDevTools()
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
    createWindow()

    app.on('activate', function () {
        // On macOS it's common to re-create a window in the app when the
        // dock icon is clicked and there are no other windows open.
        if (BrowserWindow.getAllWindows().length === 0) createWindow()
    })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
    if (!macOS) app.quit()
})

ipcMain.on("save-keymap-callback", async (ev, { remap, filePath }) => {
    let string = YAML.stringify(remap);
    await fs.writeFile(filePath, string);
});

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.