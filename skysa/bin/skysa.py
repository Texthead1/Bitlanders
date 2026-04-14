import os
import sys

from assembler import assembler_act
from out       import CA65ASM_EXT, SKYASM_EXT

VERBOSE = False
ASM_LOG_FILE = "skysa_assembled.txt"
OBJ_LIST_FILE = "skysa_objlist.txt"

def assemble(path):
    out_files = []
    obj_list = []
    print(f"Assembling SkyASM {SKYASM_EXT} files in directory: {path}")
    for root, _, files in os.walk(path):
        for filename in files:
            if filename.endswith(SKYASM_EXT):
                if VERBOSE: print("Assembling file: " + filename)
                assembler_act(os.path.join(root, filename))
                if VERBOSE: print(f"Assembled: {filename}")
                clean_root = os.path.relpath(root, ".")
                if clean_root == ".":
                    clean_root = ""

                out_files.append(os.path.join(clean_root, os.path.splitext(filename)[0] + CA65ASM_EXT))
                obj_list.append(os.path.join(clean_root, os.path.splitext(filename)[0] + ".o"))

    match (len(out_files)):
        case 0: return
        case 1: print(f"Assembled 1 SkyASM {SKYASM_EXT} file")
        case _: print(f"Assembled {len(out_files)} SkyASM {SKYASM_EXT} files")

    with open(f"{path}/{ASM_LOG_FILE}", "w") as f:
        out_str = "\n".join(out_files)
        if VERBOSE: print(f"{ASM_LOG_FILE}:\n{out_str}")
        f.write(out_str)

    with open(f"{path}/{OBJ_LIST_FILE}", "w") as f:
        obj_str = "\n".join(obj_list)
        if VERBOSE: print(f"{OBJ_LIST_FILE}:\n{obj_str}")
        f.write(obj_str)

def clean_skyasm(file):
    base_name = os.path.splitext(file)[0]
    if VERBOSE: print(f"Cleaning file: {base_name}{CA65ASM_EXT}")
    os.remove(base_name + CA65ASM_EXT)

def clean(path):
    for root, _, files in os.walk(path):
        for filename in files:
            if filename.endswith(SKYASM_EXT):
                clean_skyasm(os.path.join(root, filename))
            elif filename == ASM_LOG_FILE:
                if VERBOSE: print(f"Removing file: {filename}")
                os.remove(os.path.join(root, filename))
            elif filename == OBJ_LIST_FILE:
                if VERBOSE: print(f"Removing file: {filename}")
                os.remove(os.path.join(root, filename))

def help():
    print("Usage: python skysa.py <path> [--asmc | --clean] [--verbose]")
    print("\nOptions:")
    print("  --asmc, -a       Assemble all SkyASM .ssa files in directory")
    print("  --clean, -c      Clean all wrapped files in directory")
    print("  --verbose, -v    Enable verbose output")
    print("  --help, -h       Show this help message")
    sys.exit(0)

if __name__ == "__main__":
    args = sys.argv[1:]

    if not args or "--help" in args or "-h" in args:
        help()

    if "--verbose" in args:
        VERBOSE = True
        args.remove("--verbose")
    if "-v" in args:
        VERBOSE = True
        args.remove("-v")

    if len(args) != 2:
        print("Error: expected a path and a single action (--asmc or --clean)")
        help()

    path = args[0]
    action = args[1]
    if action in ("--asmc", "-a"):
        assemble(path)
    elif action in ("--clean", "-c"):
        clean(path)
    else:
        print(f"Unknown argument: {action}")
        help()