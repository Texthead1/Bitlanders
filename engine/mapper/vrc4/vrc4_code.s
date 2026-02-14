.segment "ZEROPAGE"
    BP_BANK_8000: .res 1
    mmc3_8000: .res 1 ; backup, just in case interrupt
    irq_done:        .res 1
    
    .exportzp BP_BANK_8000, mmc3_8000
    .exportzp mmc3_ptr, mmc3_index, irq_done
    .exportzp _irq_handler_ptr
    

.segment "STARTUP"
;needs to be mapped to the fixed bank

.export _set_prg_8000, _get_prg_8000, _set_prg_a000
.export _set_chr_mode_0, _set_chr_mode_1, _set_chr_mode_2, _set_chr_mode_3
.export _set_chr_mode_4, _set_chr_mode_5

.export _set_mirroring, _disable_irq
.export _set_irq_ptr

_set_prg_8000:
    sta BP_BANK_8000
    pha
    lda #(6 | A12_INVERT)
    bne bank_swap_common

_get_prg_8000:
    lda BP_BANK_8000
    ldx #0
    rts
    
_set_prg_a000:
    pha
    lda #(7 | A12_INVERT)
    bne bank_swap_common

_set_chr_mode_0:
    pha
    lda #(0 | A12_INVERT)
bank_swap_common:    
    sta mmc3_8000 ;backup, in case of interrupt
    sta $8000   
    pla
    sta $8001
    rts

_set_chr_mode_1:
    pha
    lda #(1 | A12_INVERT)
    bne bank_swap_common

_set_chr_mode_2:
    pha
    lda #(2 | A12_INVERT)
    bne bank_swap_common
    
_set_chr_mode_3:
    pha
    lda #(3 | A12_INVERT)
    bne bank_swap_common
    
_set_chr_mode_4:
    pha
    lda #(4 | A12_INVERT)
    bne bank_swap_common
    
_set_chr_mode_5:
    pha
    lda #(5 | A12_INVERT)
    bne bank_swap_common
    
_set_mirroring:
    sta $a000
    rts
    
_disable_irq:
    sta $e000
    lda #<default_array
    ldx #>default_array
    
_set_irq_ptr:
    sta mmc3_ptr
    stx mmc3_ptr+1
    rts    

irq:
    jmp (_irq_handler_ptr)