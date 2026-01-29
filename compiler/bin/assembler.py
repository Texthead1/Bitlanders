import os
import sys

from dependencies.isa import MNEMONIC_TO_OPCODE, OPERAND_COUNTS
from out              import CA65ASM_EXT, SKYASM_EXT, emit_proc
from tokenizer        import parse_operand, tokens_empty, tokenize

def handle_func(lines):
    global base_file

    code = []
    for line in lines:
        tokens = tokenize(line)
        if tokens_empty(tokens):
            continue

        mnemonic = tokens[0].lower()
        if mnemonic in MNEMONIC_TO_OPCODE:
            opcode = MNEMONIC_TO_OPCODE[mnemonic]
            operand_count = OPERAND_COUNTS[opcode]

            code.append(opcode)

            for i in range(operand_count):
                if len(tokens) > i + 1:
                    operand = parse_operand(tokens[i + 1])
                    code.append(operand)
                else:
                    print(f"Error: {mnemonic} expects {operand_count} operand(s)")
                    sys.exit(1)
        else:
            print(f"Error: unknown instruction: \"{mnemonic}\"")
            sys.exit(1)
    return code

# we wanna convert the SkyASM file down to the byte-level instructions
# and then wrap in 6502 asm to stream to the command port
# then ca65 will assemble the out .s
def assembler_act(file):
    base_name = os.path.splitext(file)[0]
    with open(base_name + SKYASM_EXT, "r") as f:
        lines = f.readlines()
    
    functions = []
    current_function = None
    function = []
    line_num = 1
    for line in lines:
        tokens = tokenize(line)
        if tokens_empty(tokens):
            line_num += 1
            continue

        token = tokens[0]
        if token.endswith(":"):
            if len(tokens) >= 2:
                print(f"Error: unexpected \"{' '.join(tokens[1:])}\" on line {line_num}")
                sys.exit(1)
            if current_function:
                code = handle_func(function)
                functions.append((current_function, code))
            current_function = token[:-1]
            function = []
        else:
            if current_function:
                function.append(line)
            else:
                print(f"Error: unexpected \"{line.strip()}\" on line {line_num}")
                sys.exit(1)
        line_num += 1
    
    if current_function:
        code = handle_func(function)
        functions.append((current_function, code))
        current_function = None
        function = []

    s_out = emit_proc(functions)

    with open(base_name + CA65ASM_EXT, "w") as f:
        f.write(s_out)