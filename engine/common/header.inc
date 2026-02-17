.segment "HEADER"
    .byte $4E, $45, $53, $1A
	.byte <NES_PRG_BANKS
	.byte <NES_CHR_BANKS
	.byte <NES_MIRRORING | (<NES_MAPPER<<4)
	.byte <NES_MAPPER & $F0
	.byte 1	; PRG RAM size (8 KB units)
	.res 7,0