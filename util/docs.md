# Nuphy Console

## Firmware Ver.
In device descriptor: "bcdDevice"

## Nuphy Console: Boot Up
Host: (Set_Report) 05 05 81 00 00 00
KB: ack
Host: (Get_Report) Feature,ID:5
KB: URB_Complete 05 01 02 00 00 00 00 4F

## Nuphy Console: Apply/Write
[W0] Set Report 5: 05 83 b6 00 00 00
[W1] Get Report 6
[W2] Set Report 5: 05 88 b8 00 00 00
[W3] Get Report 6
[W4] Set Report 6 (LONG, same data format as W3?)
[W5] Set Report 5: 05 84 d8 00 00 00
[W6] Get Report 6
[W7] Set Report 6 (LONG, same data format as W6) <-- keymap?
[W8] Set Report 7 (LONG, same data format as W1? (but padded))

# URB Format
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

# Keymap Format
Each key appears to be assigned a word.

The least significant byte is a scancode. The most significant byte is a qualifier of some kind.

## Qualifiers
00 -> Normal Key (uses 1st byte)
06 -> Modifier (uses 1st byte)
12 -> ?? (uses 2nd byte)
0b -> ?? (uses 2nd byte)
0c -> ?? (uses 2nd byte)
0d -> ?? (uses 2nd byte)
0e -> ?? (uses 1st byte)