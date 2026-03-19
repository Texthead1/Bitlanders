.include "../../../lib/famitone2/famitone2.inc"

_init_mapper_immediate:
    rts

_init_mapper:
    rts

_init_famitone:
    ldx #<music_data
    ldy #>music_data
    lda <NTSC_MODE
    jsr FamiToneInit

.if(FT_SFX_ENABLE)
    ldx #<sounds_data
    ldy #>sounds_data
    jsr FamiToneSfxInit
.endif
    rts

irq:
; this doesn't actually get called