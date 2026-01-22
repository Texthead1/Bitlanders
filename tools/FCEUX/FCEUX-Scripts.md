# SkyRetro FCEUX LUA Scripts
Development scripts to help with SkyRetro Engine NES game development.

## SkyRetro_Spoofer.lua
![spoofer](/assets/media/FCEUX_spoofer.png)

This script currently spoofs bytes written by the NES ROM to a given address (`SKYRETRO_CMD_PORT`), and prints out relevant debugging information respective of the bytes it spoofs.

Address `$4028` is treated as a streamed command port  for the NES to send instructions to the SkyRetro Peripheral Interface Controller (SPIC).

The format is a custom assembly language, SkyASM, compiled down by the skyas assembler into byte-level semantic instructions, which are then wrapped in 6502 asm to stream each byte over the port. SkyASM is contained within .ssa files (SkyASM Source Assembly). The SPIC then interprets the byte-level instructions and responds accordingly.

Instructions either execute internal logic (modifying registers, clearing buffers, etc) or external logic (peripheral LEDs, speakers, etc).

The output console when using this LUA script acts as a dissassembler to inspect the SkyASM assembly instructions that are streamed by the NES, formatted as so:

PC   | Address | Opcode (Hex) | Operand(s) | Opcode Mnemonic
-----|---------|--------------|------------|----------------

For example, the output console from the embedded image above would look like:

PC   | Address | Opcode (Hex) | Operand(s) | Opcode Mnemonic
-----|---------|--------------|------------|----------------
C324 | 4028    | 00           |            | RST
C329 | 4028    | 01           |            | AON
C333 | 4028    | 30           | FF         | LDR
C33D | 4028    | 31           | 00         | LDG
C347 | 4028    | 32           | 00         | LDB
C34C | 4028    | 33           |            | RGB

> [!WARNING]
> These Mnemonic and Opcode pairings are outdated in relation to the [SkyASM Opcodes](../../docs/SkyASM-Opcodes.md). However, this updated list has not yet been taken into account with this script or skyas.