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
const { contextBridge, ipcRenderer } = require("electron");

contextBridge.exposeInMainWorld('ipc', {
    onLoadConfig: (callback) => ipcRenderer.on("load-config", callback),
    onSaveConfig: (callback) => ipcRenderer.on("save-config", callback),
    getKeyboardInfo: () => ipcRenderer.send("get-keyboard-info"),
    onGetKeyboardInfo: (callback) => ipcRenderer.on("get-keyboard-info-reply", callback),
    sendConfig: (remap) => ipcRenderer.send("write-yaml", remap)
});