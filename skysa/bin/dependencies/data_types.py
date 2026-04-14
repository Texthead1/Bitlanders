from dataclasses import dataclass

@dataclass
class skyasmInstruction:
    mnemonic: str
    operands: list

@dataclass
class nativeASM:
    lines: list[str]