# defines NES registers, as well as identifiers for them in the tokenizer
NES_REGISTERS = {
    "A": 0xF1,
    "X": 0xF2,
    "Y": 0xF3
}

NES_IDENTIFIER_PREFIX = 0xFF
NES_PREFIX_COUNT = 3
NES_IDENTIFIER_LENGTH = NES_PREFIX_COUNT + 1

def get_asm_for_register_by_identifier(identifier):
    identifier -= generate_nes_register_identifier_prefix()
    if not identifier in NES_REGISTERS.values():
        return None

    match identifier:
        case 0xF1:
            return "sta"
        case 0xF2:
            return "stx"
        case 0xF3:
            return "sty"

def generate_nes_register_identifier_prefix():
    value = 0
    for i in range(NES_PREFIX_COUNT):
        value += NES_IDENTIFIER_PREFIX
        value <<= 0x08
    return value

def generate_nes_register_identifier(register_name):
    if register_name not in NES_REGISTERS:
        return None

    identifier = generate_nes_register_identifier_prefix()
    identifier += NES_REGISTERS[register_name]
    return identifier

def nes_register_identifier(bytes, index):
    return (bytes[index + 1] - generate_nes_register_identifier_prefix()) in NES_REGISTERS.values()