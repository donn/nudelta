<h1 style="text-align: center;"> νΔ / nudelta </h1>

> Note: This software is in EARLY ALPHA. I'm not responsible for anything that might happen to your keyboard.

A (WIP) open-source alternative to the [Nuphy Console](https://nuphy.com/pages/nuphy-console) for the [Air75 Mechanical Wireless Keyboard](https://nuphy.com/collections/keyboards/products/air75) by reverse-engineering the USB protocol.

Currently supports backing up and dumping keymaps.

## Requirements
* [libusb](https://github.com/libusb/libusb)

> Nudelta will not work in its current state on ppc64 or any big-endian architectures. 

### Build Requirements
* C++17 Compiler
* Cmake
* Python 3.6+

## Limitations vs. NuPhy Console
* Only the Air75 is supported.
    * Halo65/Air60 are not supported (I don't have them)
* No GUI
* Simple remaps only: no macros/combinations
* No RGB Control

## Limitations also in NuPhy Console
* Only remaps keymap labeled "WIN"

## License
The GNU General Public License v3 or, at your option, any later version. Check '[License](/License)'.
