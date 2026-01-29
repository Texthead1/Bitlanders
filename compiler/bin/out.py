import sys
from dependencies.nes import get_asm_for_register_by_identifier, nes_register_identifier

SKYASM_EXT  = ".ssa"
CA65ASM_EXT = ".s"

SKYRETRO_CMD_PORT = 0x4028

# returns a string[] representing a 6502 asm function
# which wraps the SkyASM instructions and streams them to the command port
def emit_proc(functions):
    out = []
    out.append(f".export {",".join([name for name, _ in functions])}\n")
    out.append(f".segment \"CODE\"")

    for name, code in functions:
        out.append(f"\n; callable in C as \"void {name}(void);\"")
        out.append(f"{name}:")

        i = 0
        while i < len(code):
            byte = code[i]
            if i < len(code) - 1 and nes_register_identifier(code, i):
                if byte < 0:
                    byte += 256
                if byte < 0 or byte > 255:
                    print(f"Error: invalid opcode byte: {byte}")
                    sys.exit(1)
                out.append(f"\tlda #${byte:02X}")
                out.append(f"\tsta ${SKYRETRO_CMD_PORT:04X}")
                register_asm_line = f"\t{get_asm_for_register_by_identifier(code[i + 1])} ${SKYRETRO_CMD_PORT:04X}"
                out.append(register_asm_line)
                i += 2
                continue
            if byte < 0:
                byte += 256
            if byte < 0 or byte > 255:
                print(f"Error: invalid opcode byte: {byte}")
                sys.exit(1)
            out.append(f"\tlda #${byte:02X}")
            out.append(f"\tsta ${SKYRETRO_CMD_PORT:04X}")
            i += 1

        out.append(f"\trts")
    return "\n".join(out)

# returns a string[] representing a C header file
# which declares the external functions
def emit_header(name, functions):
    out = []
    out.append(f"// Auto-generated header for {name}{SKYASM_EXT}\n")

    for func in functions:
        out.append(f"void {func}(void);")

    return "\n".join(out)