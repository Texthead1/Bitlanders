import sys
from dependencies.data_types import nativeASM, skyasmInstruction
from dependencies.isa        import MNEMONIC_TO_OPCODE
from dependencies.nes        import NES_REGISTERS, NESRegisterOperand
from parser                  import count_nes_a_register_uses

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

        nes_a_register_use_count = count_nes_a_register_uses(code)
        uses_nes_a_register = nes_a_register_use_count > 0
        a_value_preserved = False

        if uses_nes_a_register:
            out.append(f"\t; function uses NES A register, preserve its value")
            out.append(f"\tpha")
            a_value_preserved = True

        for instruction in code:
            if isinstance(instruction, nativeASM):
                out.append(f"\t; native assembly block")
                for line in instruction.lines:
                    out.append(f"\t{line}")
                continue

            if isinstance(instruction, skyasmInstruction):
                opcode = MNEMONIC_TO_OPCODE[instruction.mnemonic]
                out.append(f"\tlda #${opcode:02X}")
                out.append(f"\tsta ${SKYRETRO_CMD_PORT:04X}")

                for operand in instruction.operands:
                    if isinstance(operand, NESRegisterOperand):
                        reg = operand.register
                        if reg == "A":
                            if a_value_preserved:
                                out.append("\tpla")
                            a_value_preserved = False

                            out.append(f"\tsta ${SKYRETRO_CMD_PORT:04X}")

                            out.append("\tpha")
                            a_value_preserved = True
                        else:
                            out.append(f"\t{NES_REGISTERS[operand.register]} ${SKYRETRO_CMD_PORT:04X}")
                    else:
                        if operand < 0:
                            operand += 256
                        if operand < 0 or operand > 255:
                            print(f"Error: invalid operand byte: {operand}")
                            sys.exit(1)
                        out.append(f"\tlda #${operand:02X}")
                        out.append(f"\tsta ${SKYRETRO_CMD_PORT:04X}")

        if a_value_preserved:
            out.append(f"\t; restore NES A register value")
            out.append(f"\tpla")

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