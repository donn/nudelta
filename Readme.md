<h1 align="center"> ν∂ / nudelta </h1>

> Note: This software is in EARLY ALPHA.

A (WIP) open-source alternative to the [Nuphy Console](https://nuphy.com/pages/nuphy-console) for the [Air75 Mechanical Wireless Keyboard](https://nuphy.com/collections/keyboards/products/air75) by reverse-engineering the keyboard's USB protocol.

What this has:
* Support for NuPhy Air75 on Linux and macOS
    * Tested on Manjaro Linux and macOS 12.4 Monterey
* The ability to back up and dump keymaps to binary formats
* The ability to dump keymaps to a human-readable hex format
* Loading keymap modifications from a `.yml` configuration file
    * Includes modifying the key as well as adding meta/alt/ctrl/shift or any combination thereof as modifiers

What this DOESN'T have:
* A GUI
    * Would like to make one but it would take too much time.
* Macros
    * Need to reverse engineer those.
* Support for Halo65/Air60
    * I don't have them.
* RGB Control
    * I don't need that, personally.


## Running Requirements
* An x86, x86_64 or Apple Silicon processor
* Linux only: [libusb](https://github.com/libusb/libusb) installed

### Build Requirements
* C++17 Compiler
* Python 3.6+ w/ Cmake, Pyyaml
    * `python3 -m pip install cmake pyyaml`

## Building
```sh
git clone https://github.com/donn/nudelta
cd nudelta
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

## Usage

You will need to use **sudo** on Linux. On macOS, you will need to grant Input Monitoring permissions to whichever Terminal host you're using to run Nudelta, likely Terminal.app.

### Load a custom profile

```sh
nudelta -l ./donns_remap.yml
```

The configuration .yml file is simple: there's a top level object called "keys": which is a map of physical keys on the actual keyboard to their replacements:

```yml
keys:
    caps_lock: esc
```

See [example.yml](example.yml) for a profile and somewhat more detailed examples.

You can find a list of:
  * Replaceable keys in [res/air75_indices.yml](res/air75_indices.yml).
  * Replacement keycodes in [res/air75_keycodes.yml](res/air75_keycodes.yml).

### Reset keymap to default
```sh
nudelta -r
```

## License
The GNU General Public License v3 or, at your option, any later version. Check '[License](/License)'.
