# SkyASM

The SkyRetro Peripheral Interface Controller (SPIC) has 3 registers: `R`, `G`, and `B`.

## Opcodes

Opcode (Hex) | Size | Mnemonic                                 | Information
-------------|------|------------------------------------------|------------
00           | 1    | RST (**R**e**s**e**t**)                  | Reset the connected peripheral
01           | 1    | AON (**A**ntenna **On**)                 | Enable the nfc field antenna of the connected peripheral
10           | 1    | RGB (Set LED **R**ed **G**reen **B**lue) | Set the RGB of the peripheral's LED based on the R, G, and B registers
11           | 2    | LDR (**L**oa**d** into **R**)            | Load a value into the R register
12           | 2    | LDG (**L**oa**d** into **G**)            | Load a value into the G register
13           | 2    | LDB (**L**oa**d** into **B**)            | Load a value into the B register
14           | 1    | RGBL (**RGB** **L**eft)                  | If supported, set the peripheral's left LED based on the R, G, and B registers
15           | 1    | RGBR (**RGB** **R**ight)                 | If supported, set the peripheral's right LED based on the R, G, and B registers
16           | 1    | RGBT (**RGB** **T**rap)                  | If supported, set the peripheral's Trap Slot LED based on the R register
18           | 4    | COL (Set LED to **Col**or)               | Set the RGB of the peripheral's LED based on the operands (RGB order)
19           | 4    | COLL (**Col**or **L**eft)                | If supported, set the peripheral's left LED based on the operands (RGB order)
1A           | 4    | COLR (**Col**or **R**ight)               | If supported, set the peripheral's right LED based on the operands (RGB order)
1B           | 2    | TRP (Set **Tr**a**p** Slot)              | If supported, set the peripheral's Trap Slot LED based on the operand
20           | 1    | SON (**S**peaker **On**)                 | If supported, activates the sound/speaker on the peripheral
28           | 2    | VIB (Set **Vib**rancy)                   | If supported, configure Trap Slot vibrancy tolerance (higher value = louder audio needed to make the LED glow)
30           | 2    | ELM (**El**e**m**ent)                    | Set the peripheral's LED based on a given elemental index ([see more](#elemental-color-index))
31           | 1    | ELER (**El**ement **E**a**r**th)         | Set the peripheral's LED to the corresponding RGB for the Earth element
32           | 1    | ELWA (**El**ement **Wa**ter)             | Set the peripheral's LED to the corresponding RGB for the Water element
33           | 1    | ELAI (**El**ement **Ai**r)               | Set the peripheral's LED to the corresponding RGB for the Air element
34           | 1    | ELFI (**El**ement **Fi**re)              | Set the peripheral's LED to the corresponding RGB for the Fire element
35           | 1    | ELLF (**El**ement **Li**fe)              | Set the peripheral's LED to the corresponding RGB for the Life element
36           | 1    | ELDE (**El**ement Un**de**ad/**De**ath)  | Set the peripheral's LED to the corresponding RGB for the Undead element
37           | 1    | ELMG (**El**ement **M**a**g**ic)         | Set the peripheral's LED to the corresponding RGB for the Magic element
38           | 1    | ELTC (**El**ement **T**e**c**h)          | Set the peripheral's LED to the corresponding RGB for the Tech element
39           | 1    | ELLT (**El**ement **L**igh**t**)         | Set the peripheral's LED to the corresponding RGB for the Light element
3A           | 1    | ELDR (**El**ement **D**a**r**k)          | Set the peripheral's LED to the corresponding RGB for the Dark element
3B           | 1    | ELKS (**El**ement **K**ao**s**)          | Set the peripheral's LED to the corresponding RGB for the Kaos element
60           | 1    | INR (**In**crement **R**)                | Increment the value in the R register by 1
61           | 1    | ING (**In**crement **G**)                | Increment the value in the G register by 1
62           | 1    | INB (**In**crement **B**)                | Increment the value in the B register by 1
70           | 2    | WAIT (**Wait** ticks)                    | Wait a given amount of ticks (tick is equal to a frame - 60hz), instructions avoid this wait time unless prepended by the BUF instruction
74           | 1    | BUF (**Buf**fer Instruction)             | Add the next instruction to the buffer. If there is no wait duration, the next instruction will execute immediately
78           | 1    | CBUF (**C**lear **Buf**fer)              | Clear the instruction buffer
7A           | 1    | NOW (Execute **Now**)                    | Clear the wait time to execute the buffered instructions. Any buffered instructions will execute immediately
81           | 1    | AOFF (**A**ntenna **Off**)               | Disable the nfc field antenna of the connected peripheral
94           | 2    | FLL (**F**ade **L**ED **L**eft)          | If supported, set the peripheral's left LED based on the R, G, and B registers, with a fade duration (tick is equal to a frame - 60hz)
95           | 2    | FLR (**F**ade **L**ED **R**ight)         | If supported, set the peripheral's right LED based on the R, G, and B registers, with a fade duration (tick is equal to a frame - 60hz)
98           | 1    | BCOL (**B**asic **Col**or Cycle)         | If supported, set the peripheral's LED to revert to its basic hardware color cycle
A0           | 1    | SOFF (**S**peaker **Off**)               | If supported, deactives the sound/speaker on the peripheral
E0           | 1    | DER (**De**crement **R**)                | Decrement the value in the R register by 1
E1           | 1    | DEG (**De**crement **G**)                | Decrement the value in the G register by 1
E2           | 1    | DEB (**De**crement **B**)                | Decrement the value in the B register by 1
F0           | 1    | WAIL (Fetch **Wai**t **L**ength)         | Get the amount of wait ticks remaining, fetch via `SKYRETRO_CMD_OUT`
F4           | 1    | BUFL (Fetch **Buf**fer **L**ength)       | Get the amount of instructions in the buffer, fetch via `SKYRETRO_CMD_OUT`
FA           | 1    | NOP (**N**o **Op**eration)               | No operation

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
- `60` (`0110 0000`) is INR (increment R register)
- `E0` (`1110 0000`) is DER (decrement R register)
    - Both share the same category, and have the same action, but differ in bit 7.

#### Categories (bits 6-4)
ID  | Category
----|---------
000 | System/Peripheral Control
001 | LEDs/Visual Output
010 | Speakers/Audio Output
011 | Peripheral Reserved (Skylander Portal of Power)
100 | Peripheral Reserved
101 | Peripheral Reserved
110 | Register Arithmetic
111 | SPIC Control

#### Low Nibble
The lower nibble selects the action within the category.

Examples from category `001` (LEDs/Visual Output):

Opcode (Hex) | Mnemonic | Category | Action
-------------|----------|----------|-------
10           | RGB      | 010      | 0000
11           | LDR      | 010      | 0001
12           | LDG      | 010      | 0010
13           | LDB      | 010      | 0011
18           | COL      | 010      | 1000
98           | BCOL     | 010      | 1000

Note that `18` and `98` share the same action (`1000`), but differ in bit 7.

## Addressing Modes
### Immediate value 
Prepending the operand with `#` indicates an immediate value
- For instance, `LDR #$40` would load `0x40` into the R register.

### Value from address
Not prepending `#` to the value, or not using a register, loads the value from the address.
- For instance, `LDB $80` would load the value at address `0x80` into the B register.

### NES Register Addressing
Instead of an integer, the operand can be substituted for one of the three NES registers, `A`, `X`, and `Y`.

`'A'`/`'X'`/`'Y'` or `"A"`/`"X"`/`"Y"` loads the value in the respective register.
- For instance, `LDG 'X'` would load the value stored in the `X` register into the G register.

> [!Note]
> Prepending the integer value with a `$` indicates hex, and `%` indicates binary. Otherwise, the value is treated as base-10. Negative base-10 numbers are also supported,

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

Using `ELM #$00` will likely set the LED to RGB `#000000`, unless a proper entry is added to the firmware for element `0`. Using values above `0B` is normally undefined behavior and will do nothing.

Additional elements can be taken advantage of with this system, given:
- A new color entry is made for said additional element (e.g. `#$OC`)
- The new entry is correctly called by the NES ROM
    - e.g., `ELM #$0C` to set the peripheral's LED to the aforementioned new color entry

> [!NOTE]
> The SPIC interprets the byte-level instruction for the elements by the high and low nibbles independently. This means that the byte-level instructions `3C`-`3F` would correlate to the same outcome as `ELM #$0C`-`ELM #$0F`. These are illegal instructions, but can be taken advantage of regardless.