# 0.6.6
- Fixed a bug where the Del key was not remappable on Air75.
- Fix scancode for Pause/Break key.
- Add scancode for Menu key. 

# 0.6.5
- Fixed a bug with accessing the clipboard on Linux.

# 0.6.3, 0.6.4
- Fixed a bug with the Halo75 where Windows and Mac modes have been switched around.

# 0.6.2
- Fixes detecting the Halo75.

# 0.6.1
- Numerous critical bugfixes and better handling for unrecognized keyboards.
- Update build system to fix odd Windows 11 bug.

# 0.6.0 (Unpublished)
- Adds support for Halo75.
- CLI now remaps Windows and Mac modes simultaneously.

# 0.5.1
- The Fn key is now remappable, however, in the GUI, you will be required to click on the Fn key five times in a row before this is enabled. Remapping the Fn key will change your factory reset shortcut and thus is only advised if you absolutely know what you're doing.

# 0.5.0
- Adds support for Windows 10+, as well as adding Windows builds to CI.
- Fixes a number of bugs and memory errors.

# 0.4.1
- UI tweaks and lint fixes.

# 0.4.0
- Allows remapping the Mac mode of the Air75.

# 0.3.1
- Tweaks build system.

# 0.3.0
- Adds an Electron-based GUI
- Adds a CI with macOS `.app` and Linux `AppImage` releases.

# 0.2.0
- Adds the ability to remap select keys on the (limited) Fn layer in the CLI app
- Added remap targets for functions handled by the Fn-layer: changing backlight brightness, etc.
- Adds support for a "raw" keycode specifier in remapping yaml files.
- Adds some more findings to documentation.

# 0.1.0
Initial release. CLI-only app with remapping for Air75's Windows mode.