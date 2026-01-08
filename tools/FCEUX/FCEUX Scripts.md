# SkyRetro FCEUX LUA Scripts
Development scripts to help with SkyRetro Engine NES game development.

## SkyRetro Spoofer.lua
![spoofer](/assets/media/FCEUX_spoofer.png)

Address $4028 is treated as a streamed machine code port for the NES to send machine code to the SkyRetro microcontroller.

The format is a custom ISA, skyasm assembly, compiled down by the skyc compiler into machine code hex, which is then wrapped in 6502 ASM to stream each byte over the port. skyasm is contained within .ssa files.

The output console when using the spoofer acts as a dissassembler to inspect the skyasm assembly, formatted as so:

PC   | Address | Opcode (Hex) | Operand 1 | Operand 2 | Operand 3 | Opcode Mnemonic
-----|---------|--------------|-----------|-----------|-----------|----------------
C327 | 4028    | 00           |           |           |           | RST
C327 | 4028    | 01           |           |           |           | AON
C327 | 4028    | 30           | FF        |           |           | LDR
C327 | 4028    | 31           | 00        |           |           | LDG
C327 | 4028    | 32           | 00        |           |           | LDB
C327 | 4028    | 33           |           |           |           | RGB

The table here has example data taken from the embedded image above.