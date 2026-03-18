.include "../../../lib/famitone2/famitone2.inc"

.include "mmc3_code.s"

.define SOUND_BANK 12

_init_mapper_immediate:
    jsr _disable_irq
    rts

_init_mapper:
    lda #0
    jsr _set_prg_8000

    lda #13
    jsr _set_prg_a000

    lda #0
    jsr _set_chr_mode_2

    lda #1
    jsr _set_chr_mode_3

    lda #2
    jsr _set_chr_mode_4

    lda #3
    jsr _set_chr_mode_5

    lda #4
    jsr _set_chr_mode_0

    lda #6
    jsr _set_chr_mode_1

    lda #0
    jsr _set_mirroring

    lda #$80
    jsr _set_wram_mode

    cli
    rts

_init_famitone:
    lda #SOUND_BANK
    jsr _set_prg_8000

    ldx #<music_data
    ldy #>music_data
    lda NTSC_MODE
    jsr FamiToneInit

.if(FT_SFX_ENABLE)
    ldx #<sounds_data
    ldy #>sounds_data
    jsr FamiToneSfxInit
.endif

    lda #0
    jsr _set_prg_8000
    rts

.segment "ONCE"
.segment "BANK0"
.segment "BANK1"
.segment "BANK2"
.segment "BANK3"
.segment "BANK4"
.segment "BANK5"
.segment "BANK6"
.segment "BANK7"
.segment "BANK8"
.segment "BANK9"
.segment "BANK10"
.segment "BANK11"
.segment "BANK12"