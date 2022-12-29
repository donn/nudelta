# NuPhy Console
## Firmware Ver.
In device descriptor: "bcdDevice"

## NuPhy Console: Boot Up
Host: (Set_Report) 05 05 81 00 00 00
KB: ack
Host: (Get_Report) Feature,ID:5
KB: URB_Complete 05 01 02 00 00 00 00 4F

## NuPhy Console: Apply/Write

```
[W0] Set Report 5: 05 83 b6 00 00 00

[W1] Get Report 6

[W2] Set Report 5: 05 88 b8 00 00 00

[W3] Get Report 6

[W4] Set Report 6 (LONG, same data format as W3?)

[W5] Set Report 5: 05 84 d8 00 00 00

[W6] Get Report 6

[W7] Set Report 6 (LONG, same data format as W6) <-- keymap

[W8] Set Report 7 (LONG, same data format as W1? (but padded))
```



# Keymap Format
Each key appears to be assigned a word.

## Qualifiers
## Byte 0
* 00 -> Normal Key
* 02 -> Unknown: Used for exactly 34 keys. Seems to be generating redundant keycode: 1, 2, q...
* 04 -> Multimedia Key
* 06 -> Modifier Key
    - Curiously, modifier scancodes appear to work just fine without it from what I see.
* 0b -> Backlight Effect
* 0c -> Backlight Intensity
* 0d -> Unknown: Two keys, appearing to be cycling through something i do not know
* 0e -> A variety of on-keyboard functions / """"Fn Layer""""
* 12 -> Backlight Color
* 20 -> Fn Key only

## Byte 1
Appears unused, always 00

## Byte 2
### For normal keys:

Modifiers: OR between:

* 0001 (ctrl)
* 0010 (shift)
* 0100 (alt)
* 1000 (meta)

Four upper bits appear unused.

### Backlight Intensity
* 01: Decrease Backlight
* 02: Increase Backlight

### Backlight Effects/Color
* 02: Cycle Backward
* 03: Cycle Forward

## Byte 3
### 0e
* 0c -> Bluetooth Device 1
* 0d -> Bluetooth Device 2
* 0e -> Bluetooth Device 3
* 0f -> 2.4 GHz Receiver
* 11-16 -> No damn clue

### everything else
A **scancode**. See https://wiki.archlinux.org/title/Keyboard_input

# Finding scancodes
Use `evtest` on Linux.

Note that there are two events: one for the keyboard proper and one for multimedia keys.

# Appendix: URB Format
```c
struct urb
{
    uint64_t id;
    uint8_t urb_type;
    uint8_t urb_transfer_type;

    uint8_t endpoint_and_direction;
    uint8_t device;
    uint16_t bus_id;
    uint8_t device_setup_req;
    uint8_t data_status;

    uint64_t sec;
    uint32_t usec;

    uint32_t status;
    uint32_t urb_length;
    uint32_t data_length;

    uint64_t setup_data;

    uint32_t interval;
    uint32_t start_frame;

    uint32_t transfer_flags;
    uint32_t iso_descriptors;

    uint8_t *data;
};
```
