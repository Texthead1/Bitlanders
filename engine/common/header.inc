.segment "HEADER"
    .byte $4E, $45, $53, $1A
    .byte <NES_PRG_BANKS
    .byte <NES_CHR_BANKS
    .byte <NES_MIRRORING | (<NES_MAPPER<<4)
    .byte <NES_MAPPER & $F0
    .byte <NES_PRG_RAM_BANKS
    .res 7,0