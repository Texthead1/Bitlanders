from dataclasses import dataclass

NES_REGISTERS = {
    "A": "sta",
    "X": "stx",
    "Y": "sty"
}

@dataclass
class NESRegisterOperand:
    register: str