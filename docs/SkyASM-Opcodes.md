# SkyASM Opcodes

The SkyRetro Peripheral Interface Controller (SPIC) has 3 registers: `R`, `G`, and `B`.

Opcode (Hex) | Size | Mnemonic | Information
-------------|------|----------|------------
00           | 1    | AON      | Enable the nfc field antenna of the connected peripheral
10           | 1    | RST      | Reset the connected peripheral
20           | 1    | RGB      | Set the RGB of the peripheral's LED based on the R, G, and B registers
21           | 2    | LDR      | Load a value into the R register
22           | 2    | LDG      | Load a value into the G register
23           | 2    | LDB      | Load a value into the B register
28           | 4    | COL      | Set the RGB of the peripheral's LED based on the operands (RGB order)
30           | 2    | ELM      | Set the peripheral's LED based on a given elemental index ([see more](#elemental-color-index))
31           | 2    | ELER     | Set the peripheral's LED to the corresponding RGB for the Earth element
32           | 2    | ELWA     | Set the peripheral's LED to the corresponding RGB for the Water element
33           | 2    | ELAI     | Set the peripheral's LED to the corresponding RGB for the Air element
34           | 2    | ELFI     | Set the peripheral's LED to the corresponding RGB for the Fire element
35           | 2    | ELLF     | Set the peripheral's LED to the corresponding RGB for the Life element
36           | 2    | ELDE     | Set the peripheral's LED to the corresponding RGB for the Undead element
37           | 2    | ELMG     | Set the peripheral's LED to the corresponding RGB for the Magic element
38           | 2    | ELTC     | Set the peripheral's LED to the corresponding RGB for the Tech element
39           | 2    | ELLT     | Set the peripheral's LED to the corresponding RGB for the Light element
3A           | 2    | ELDR     | Set the peripheral's LED to the corresponding RGB for the Dark element
3B           | 2    | ELKS     | Set the peripheral's LED to the corresponding RGB for the Kaos element
40           | 1    | RGBL     | If supported, set the peripheral's left LED based on the R, G, and B registers
41           | 1    | RGBR     | If supported, set the peripheral's right LED based on the R, G, and B registers
42           | 1    | RGBT     | If supported, set the peripheral's Trap Slot LED based on the R register
48           | 4    | COLL     | If supported, set the peripheral's left LED based on the operands (RGB order)
49           | 4    | COLR     | If supported, set the peripheral's right LED based on the operands (RGB order)
4A           | 2    | TRP      | If supported, set the peripheral's Trap Slot LED based on the operand
50           | 1    | INR      | Increment the value in the R register by 1
51           | 1    | ING      | Increment the value in the G register by 1
52           | 1    | INB      | Increment the value in the B register by 1
60           | 1    | SON      | If supported, activates the sound/speaker on the peripheral
68           | 2    | VIB      | If supported, configure Trap Slot vibrancy tolerance (higher value = louder audio needed to make the LED glow)
70           | 2    | WAIT     | Wait a given amount of ticks (tick is equal to a frame - 60hz), instructions avoid this wait time unless prepended by the BUF instruction
74           | 1    | BUF      | Add the next instruction to the buffer. If there is no wait duration, the next instruction will execute immediately
7A           | 1    | NOW      | Clear the wait time to execute the buffered instructions. Any buffered instructions will execute immediately
80           | 1    | AOFF     | Disable the nfc field antenna of the connected peripheral
A8           | 1    | BCOL     | If supported, set the peripheral's LED to revert to its basic hardware color cycle
C0           | 2    | FLL      | If supported, set the peripheral's left LED based on the R, G, and B registers, with a fade duration (tick is equal to a frame - 60hz)
C1           | 2    | FLR      | If supported, set the peripheral's right LED based on the R, G, and B registers, with a fade duration (tick is equal to a frame - 60hz)
D0           | 1    | DER      | Decrement the value in the R register by 1
D1           | 1    | DEG      | Decrement the value in the R register by 1
D2           | 1    | DEB      | Decrement the value in the R register by 1
E0           | 1    | SOFF     | If supported, deactives the sound/speaker on the peripheral
F8           | 2    | CBUF     | Clear the instruction buffer
F0           | 1    | WAIL     | Get the amount of wait ticks remaining, fetch via SKYRETRO_CMD_OUT
F4           | 1    | BUFL     | Get the amount of instructions in the buffer, fetch via SKYRETRO_CMD_OUT
FA           | 1    | NOP      | No operation

## Byte-level instruction encoding
The SkyASM byte-level semantic instructions are constructed in hex as so:

Bits     | Meaning
---------|--------
Bit 7    | Logical alternate flag
Bits 6-4 | Category (0-7)
Bits 3-0 | Action within Category (0-15)

This structure gives each category up to 16 actions, and each action has 2 logical variants (primary and alternate).

### High Nibble
The high nibble is divided into:

#### Logical Alternate flag (bit 7)
- `0` uses the primary category
- `1` uses the alternate category

The alternate category contains logically relevant alternatives, either inverted or complimentary instructions. For example:
- `50` (`0101 0000`) is INR (increment R register)
- `D0` (`1101 0000`) is DER (decrement R register)
    - Both share the same category, and have the same action, but differ in bit 7.

#### Categories (bits 6-4)
ID  | Category
----|---------
000 | Antenna/Peripheral Control
001 | System/Reset
010 | LEDs
011 | Elemental Color Operations
100 | Extended LEDs
101 | Register Arithmetic
110 | Speaker/Audio
111 | SPIC Control

#### Low Nibble
The lower nibble selects the action within the category.

Examples from category `010` (LED / RGB):

Opcode (Hex) | Mnemonic | Category | Action
-------------|----------|----------|-------
20           | RGB      | 010      | 0000
21           | LDR      | 010      | 0001
22           | LDG      | 010      | 0010
23           | LDB      | 010      | 0011
28           | COL      | 010      | 1000
A8           | BCOL     | 010      | 1000

Note that `28` and `A8` share the same action (`1000`), but differ in bit 7.

## Addressing Modes
`#` loads an immediate value
> For instance, `LDR #$40` would load `0x40` into the R register.

`"A"`/`"X"`/`"Y"` loads the value in the current register.
> For instance, `LDG "A"` would load the value stored in the `A` register into the G register.

Not prepending `#` to the value, or not using a register, loads the value from the address.
> For instance, `LDB $80` would load the value at address `0x80` into the B register.

> [!Note]
> Prepending the integer value with a `$` indicates hex, and `%` indicates binary. Otherwise, the value is treated as base-10.

## Elemental Color Index
The SPIC's firmware itself contains RGB entries for each element, so that the peripheral's LED can be changed to a predefined color based on element. This primarily exists for keeping elemental color consistency if the SPIC's firmware is being simulated and/or embedded inside of another project that houses its own elemental colors.

Each element has a given numerical value.
ID | Element
---|--------
00 | None
01 | Earth
02 | Water
03 | Air
04 | Fire
05 | Life
06 | Undead
07 | Magic
08 | Tech
09 | Light
0A | Dark
0B | Kaos

As an example, using the instruction `ELM #$04` would give the same outcome as `ELFI` (set LED to the Fire element color).

Using `ELM #$00` will set the LED to RGB `#000000`, unless a proper entry is added to the firmware for element `0`. Using values above `0B` is normally undefined behavior and will do nothing.

Additional elements can be taken advantage of with this system, given:
- A new color entry is made for said additional element (e.g. `OC`)
- The new entry is correctly called by the NES ROM
    - e.g., `ELM #$0C` to set the peripheral's LED to the aforementioned new color entry

> [!NOTE]
> The SPIC interprets the byte-level instruction for the elements by the high and low nibbles independently. This means that the byte-level instructions `3C`-`3F` would correlate to the same outcome as `ELM #$0C`-`ELM #$0F`. These are illegal instructions, but can be taken advantage of regardless.