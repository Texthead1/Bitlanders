import os
import sys

from parser           import parse
from out              import CA65ASM_EXT, SKYASM_EXT, emit_proc
from tokenizer        import strip_comments, token_amount_not_expected, tokens_empty, tokenize

# we wanna convert the SkyASM file down to identifiers
# and then wrap in 6502 asm to stream to the command port
# then ca65 will assemble the out .s
def assembler_act(file):
    base_name = os.path.splitext(file)[0]
    with open(base_name + SKYASM_EXT, "r") as f:
        lines = f.readlines()
    
    functions = []
    current_function = None
    function = []
    native_asm = False
    line_num = 1

    for line in lines:
        tokens = tokenize(line)
        stripped_line = strip_comments(line)
        if tokens_empty(tokens):
            line_num += 1
            continue

        if stripped_line == ".native":
            native_asm = True
        elif stripped_line == ".endnative":
            native_asm = False

        if native_asm:
            if current_function:
                function.append(line)
            else:
                print(f"Error: unexpected .native outside of function on line {line_num}")
                sys.exit(1)
            line_num += 1
            continue

        token = tokens[0]
        if token.endswith(":"):
            if token_amount_not_expected(tokens, 1):
                print(f"Error: unexpected \"{' '.join(tokens[1:])}\" on line {line_num}")
                sys.exit(1)
            if current_function:
                code = parse(function)
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
        code = parse(function)
        functions.append((current_function, code))

    s_out = emit_proc(functions)

    with open(base_name + CA65ASM_EXT, "w") as f:
        f.write(s_out)