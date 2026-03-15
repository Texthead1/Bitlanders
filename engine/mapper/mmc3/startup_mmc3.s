.define SOUND_BANK 12

.segment "STARTUP"
    .export _set_prg_8000

_set_prg_8000:
    sta BP_BANK_8000
    pha
    lda #(6 | A12_INVERT)
    bne bank_swap_common