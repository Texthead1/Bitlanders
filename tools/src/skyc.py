import os
import sys

# currently using a simplified function declaration labeling system
# .fn function_name
#   <skyasm code>
# .endfn
#
# TODO: make this more robust with a proper parser
# TODO: detect duplicate function names
# TODO: better error handling

SKYASM_EXT  = ".ssa"
CA65ASM_EXT = ".s"

SKYRETRO_CMD_PORT = 0x4028
VERBOSE = False

base_name = ""

# TODO: expand opcodes
OPCODES = {
    "rst" : [0x00],
    "aon" : [0x01],
    "ldr" : lambda x: [0x30, x],
    "ldg" : lambda x: [0x31, x],
    "ldb" : lambda x: [0x32, x],
    "rgb" : [0x33],
    "col": lambda r, g, b: [0x34, r, g, b],
    "nop" : [0xea],
}

def parse_operand(operand):
    operand = operand.strip()
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
        return int(operand)

def compile_function(lines):
    global base_file

    mcode = []
    for line in lines:
        line = line.strip().split(";")[0]
        if not line: continue
        
        tokens = line.split()
        if VERBOSE: print(tokens)
        
        op = tokens[0].lower()
        if op in OPCODES:
            val = OPCODES[op]
            if callable(val):
                operand = parse_operand(tokens[1]) if len(tokens) > 1 else 0
                mcode.extend(val(operand))
            else:
                mcode.extend(val)
        else:
            print(f"Unknown opcode: {op}")
            sys.exit(1)
    return mcode

# returns a string[] representing a 6502 asm function
# which wraps the skyasm machine code as .byte directives
def emit_proc(name, mcode):
    out = []
    out.append(f".export _{name}\n")		
    out.append(f".segment \"CODE\"\n")
    out.append(f"; callable in C as: void {name}(void);")
    out.append(f"_{name}:")
    out.append(f"\tldx #$00")
    out.append(f"@loop:")
    out.append(f"\tlda {name}_data, x")
    out.append(f"\tsta ${SKYRETRO_CMD_PORT:04X}")
    out.append(f"\tinx")
    out.append(f"\tcpx #{len(mcode)}")
    out.append(f"\tbne @loop")
    out.append(f"\trts\n")
    out.append(f"{name}_data:")

    for byte in mcode:
        out.append(f"\t.byte ${byte:02X}")

    return "\n".join(out)

# returns a string[] representing a C header file
# which declares the external functions
def emit_header(name, functions):
    out = []
    out.append(f"// Auto-generated header for {name}{SKYASM_EXT}\n")

    for func in functions:
        out.append(f"void {func}(void);")
    
    return "\n".join(out)

# we wanna convert the skyasm file to 6502 asm
# out a .s file which wraps the skyasm to 6502 asm
# then ca65 will assemble the out .s
def wrap_skyasm():
    global base_name

    if VERBOSE: print(f"Handling file: {base_name}{SKYASM_EXT}")
    with open(base_name + SKYASM_EXT, "r") as f:
        lines = f.readlines()
    
    functions = []
    current_function = None
    function_lines = []
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
                function_lines = []
            case ".endfn":
                if current_function:
                    mcode = compile_function(function_lines)
                    functions.append((current_function, mcode))
                    current_function = None
                    function_lines = []
                else:
                    print(f"Error: unexpected .endfn on line {line_num}")
                    sys.exit(1)
            case _:
                if current_function:
                    function_lines.append(line)
                else:
                    print (f"Error: unexpected line outside function: \"{line}\" on line {line_num}")
                    sys.exit(1)
        line_num += 1

    if current_function:
        print(f"Error: unresolved function \"{current_function}\"; missing .endfn")
        sys.exit(1)

    s_out = []
    for func_name, mcode in functions:
        s_out.append(emit_proc(func_name, mcode))

    with open(base_name + CA65ASM_EXT, "w") as f:
        f.write("\n".join(s_out))

def handle_file(file):
    global base_name

    base_name = os.path.splitext(file)[0]
    wrap_skyasm()

def assemble():
    out_files = []
    obj_list = []
    for root, dirs, files in os.walk('.'):
        for filename in files:
            if filename.endswith(SKYASM_EXT):
                print("Assembling file: " + filename)
                handle_file(os.path.join(root, filename))
                print(f"Assembled: {filename}")
                out_files.append(os.path.join(root.removeprefix(".\\"), os.path.splitext(filename)[0] + CA65ASM_EXT))
                obj_list.append(os.path.join(root.removeprefix(".\\"), os.path.splitext(filename)[0] + ".o"))

    print(f"Assembled {len(out_files)} files.")
    if len(out_files) == 0: return
    
    with open("skyc_assembled.txt", "w") as f:
        print("\n".join(out_files))
        f.write("\n".join(out_files))
    
    with open("skyc_objlist.txt", "w") as f:
        print("\n".join(obj_list))
        f.write("\n".join(obj_list))

# we wanna grab the original .sky.s file and remove the wrapped .s file
# then change the extension of the original from .sky.s to .s
def clean_skyasm(file):
    base_name = os.path.splitext(file)[0]
    if VERBOSE: print(f"Cleaning file: {base_name}{CA65ASM_EXT}")
    os.remove(base_name + CA65ASM_EXT)

def clean():
    for root, dirs, files in os.walk('.'):
        for filename in files:
            if filename.endswith(SKYASM_EXT):
                clean_skyasm(os.path.join(root, filename))
            elif filename == "skyc_assembled.txt":
                if VERBOSE: print(f"Removing file: {filename}")
                os.remove(filename)
            elif filename == "skyc_objlist.txt":
                if VERBOSE: print(f"Removing file: {filename}")
                os.remove(filename)

def help():
    print("Usage: python skyc.py [--asmc | --clean] [--verbose]")
    print("  --asmc, -a     : Assemble all skyasm files in directory.")
    print("  --clean, -c    : Clean all wrapped files in directory.")
    print("  --verbose, -v  : Enable verbose output.")
    print("  --help, -h     : Show this help message.")
    sys.exit(0)

if __name__ == "__main__":
    if (len(sys.argv) < 2 or len(sys.argv) > 3 or sys.argv[1] in ["--help", "-h"]):
        help()
    if len(sys.argv) == 3:
        if sys.argv[2] in ["--verbose", "-v"]:
            VERBOSE = True
        else:
            print(f"Unknown argument: {sys.argv[2]}")
            sys.exit(1)

    match sys.argv[1]:
        case "--asmc":
            assemble()

        case "-a":
            assemble()

        case "--clean":
            clean()
        
        case "-c":
            clean()

        case _:
            print(f"Unknown argument: {sys.argv[1]}")