import os
import sys

from assembler import assembler_act
from out import CA65ASM_EXT, SKYASM_EXT

# TODO: detect duplicate function names
# TODO: better error handling
# TODO: add inline 6502 asm (PHA and PLA are musts)

VERBOSE = False
ASM_LOG_FILE = "skyas_assembled.txt"
OBJ_LIST_FILE = "skyas_objlist.txt"

def assemble():
    out_files = []
    obj_list = []
    for root, _, files in os.walk('.'):
        for filename in files:
            if filename.endswith(SKYASM_EXT):
                if VERBOSE: print("Assembling file: " + filename)
                assembler_act(os.path.join(root, filename))
                if VERBOSE: print(f"Assembled: {filename}")
                out_files.append(os.path.join(root.removeprefix(".\\"), os.path.splitext(filename)[0] + CA65ASM_EXT))
                obj_list.append(os.path.join(root.removeprefix(".\\"), os.path.splitext(filename)[0] + ".o"))

    match (len(out_files)):
        case 0: return
        case 1: print(f"Assembled 1 SkyASM {SKYASM_EXT} file")
        case _: print(f"Assembled {len(out_files)} SkyASM {SKYASM_EXT} files")

    with open(ASM_LOG_FILE, "w") as f:
        if VERBOSE: print("\n".join(out_files))
        f.write("\n".join(out_files))

    with open(OBJ_LIST_FILE, "w") as f:
        if VERBOSE: print("\n".join(obj_list))
        f.write("\n".join(obj_list))

def clean_skyasm(file):
    base_name = os.path.splitext(file)[0]
    if VERBOSE: print(f"Cleaning file: {base_name}{CA65ASM_EXT}")
    os.remove(base_name + CA65ASM_EXT)

def clean():
    for root, _, files in os.walk('.'):
        for filename in files:
            if filename.endswith(SKYASM_EXT):
                clean_skyasm(os.path.join(root, filename))
            elif filename == ASM_LOG_FILE:
                if VERBOSE: print(f"Removing file: {filename}")
                os.remove(filename)
            elif filename == OBJ_LIST_FILE:
                if VERBOSE: print(f"Removing file: {filename}")
                os.remove(filename)

def help():
    print("Usage: python skyas.py [--asmc | --clean] [--verbose]")
    print("  --asmc, -a     : Assemble all SkyASM .ssa files in directory")
    print("  --clean, -c    : Clean all wrapped files in directory")
    print("  --verbose, -v  : Enable verbose output")
    print("  --help, -h     : Show this help message")
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