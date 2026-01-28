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
            print(f"Unknown opcode: {mnemonic}")
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
        line = line.strip()
        if not line or line.startswith(";"):
            line_num += 1
            continue
        
        head = line.split(";")[0].split()[0].strip()
        name = line.split(";")[0].split()[1].strip() if len(line.split(";")[0].split()) > 1 else ""
        if not name and head == ".fn":
            print(f"Error: missing function name on line {line_num}")
            sys.exit(1)

        match head:
            case ".fn":
                current_function = name
                function = []
            case ".endfn":
                if current_function:
                    code = handle_func(function)
                    functions.append((current_function, code))
                    current_function = None
                    function = []
                else:
                    print(f"Error: unexpected .endfn on line {line_num}")
                    sys.exit(1)
            case _:
                if current_function:
                    function.append(line)
                else:
                    print (f"Error: unexpected line outside function: \"{line}\" on line {line_num}")
                    sys.exit(1)
        line_num += 1

    if current_function:
        print(f"Error: unresolved function \"{current_function}\"; missing .endfn")
        sys.exit(1)

    s_out = []
    for func_name, code in functions:
        s_out.append(emit_proc(func_name, code))

    with open(base_name + CA65ASM_EXT, "w") as f:
        f.write("\n".join(s_out))