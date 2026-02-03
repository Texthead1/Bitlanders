import sys

from dependencies.data_types import nativeASM, skyasmInstruction
from dependencies.isa        import MNEMONIC_TO_OPCODE, OPERAND_COUNTS
from dependencies.nes        import NES_REGISTERS, NESRegisterOperand
from tokenizer               import strip_comments, token_amount_not_expected, tokenize, tokens_empty

def count_nes_a_register_uses(code):
    count = 0
    for instruction in code:
        if isinstance(instruction, skyasmInstruction):
            for op in instruction.operands:
                if isinstance(op, NESRegisterOperand) and op.register == "A":
                    count += 1
    return count

def parse_operand(operand):
    operand = operand.strip()

    if operand.strip("'") in NES_REGISTERS:
        return NESRegisterOperand(operand.strip("'"))
    elif operand.strip('"') in NES_REGISTERS:
        return NESRegisterOperand(operand.strip('"'))

    if operand.startswith("#$"):
        return int(operand[2:], 16)
    elif operand.startswith("#%"):
        return int(operand[2:], 2)
    elif operand.startswith("$"):
        return int(operand[1:], 16)
    elif operand.startswith("#"):
        return int(operand[1:], 10)
    elif operand.startswith("%"):
        return int(operand[1:], 2)
    else:
        print (f"Error: invalid operand: {operand}")
        sys.exit(1)

def parse(lines):
    code = []
    native_asm = False
    native_block_lines = []
    for line in lines:
        tokens = tokenize(line)
        line = strip_comments(line)

        if tokens_empty(tokens):
            continue

        if line == ".native":
            if native_asm:
                print("Error: already in .native block")
                sys.exit(1)
            if token_amount_not_expected(tokens, 1):
                print(f"Error: unexpected {' '.join(tokens[1:])} after {tokens[0]}")
                sys.exit(1)
            native_asm = True
            continue

        if line == ".endnative":
            if not native_asm:
                print("Error: not in .native block")
                sys.exit(1)
            if token_amount_not_expected(tokens, 1):
                print(f"Error: unexpected {' '.join(tokens[1:])} after {tokens[0]}")
                sys.exit(1)
            native_asm = False
            code.append(nativeASM(native_block_lines))
            native_block_lines = []
            continue

        if native_asm:
            native_block_lines.append(line.strip())
            continue

        mnemonic = tokens[0].lower()
        if mnemonic in MNEMONIC_TO_OPCODE:
            operand_count = OPERAND_COUNTS[MNEMONIC_TO_OPCODE[mnemonic]]

            if token_amount_not_expected(tokens, operand_count + 1):
                print(f"Error: {mnemonic} expects {operand_count} operand(s)")
                sys.exit(1)
            operands = []
            for i in range(operand_count):
                operand = parse_operand(tokens[i + 1])
                operands.append(operand)

            code.append(skyasmInstruction(mnemonic, operands))
        else:
            print(f"Error: unknown instruction: \"{mnemonic}\"")
            sys.exit(1)
    return code