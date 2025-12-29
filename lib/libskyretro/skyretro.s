.segment "ZEROPAGE"

.export _emu_flags
.export _emu_flags_prev

_emu_flags:      .res 1
_emu_flags_prev: .res 1

.segment "CODE"

; translate c code implementations to asm :>