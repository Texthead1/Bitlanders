# TODO:
# add comment removal, detect importance per line
# detect multiple opcodes
# change label system

import sys
from dependencies.nes import NES_REGISTERS, generate_nes_register_identifier

def strip_comments(line):
    return line.split(";")[0].strip()

def line_is_empty(line):
    return not line.strip()

def tokenize(line):
    line = strip_comments(line)
    if line_is_empty(line):
        return []
    return line.split()

def tokens_empty(tokens):
    return not tokens

def parse_operand(operand):
    operand = operand.strip()

    if operand.strip("'") in NES_REGISTERS:
        return generate_nes_register_identifier(operand.strip("'"))
    elif operand.strip('"') in NES_REGISTERS:
        return generate_nes_register_identifier(operand.strip('"'))

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
    elif operand.isdigit():
        return int(operand, 10)
    elif operand.startswith("-") and operand[1:].isdigit():
        return int(operand, 10)
    else:
        print (f"Error: invalid operand: {operand}")
        sys.exit(1)