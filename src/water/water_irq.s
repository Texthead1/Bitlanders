.include "../../include/common.inc"

.import _water_palette
.import irq_handler_ptr
.importzp _cam_latch_x
.importzp _cam_latch_y

.segment "CODE"
.export _water_irq

.define WATER_SURFACE #$01

_water_irq:
    pha
    txa
    pha
    tya
    pha

    lda #$00
    ldx #$3F
    ldy WATER_SURFACE

    sta $E000

    nop_for 36

    sta PPU_MASK
    stx PPU_ADDR
    sta PPU_ADDR
    sty PPU_DATA

    sta BANK_REG
    lda #$12
    ldy #%10001110
    sty PPU_MASK
    sta CHR_ID_REG

    ldy _cam_latch_x
    sty PPU_SCROLL
    ldy _cam_latch_y
    sty PPU_SCROLL

    nop_for 35

    sta BANK_REG
    lda #$08
    sta CHR_ID_REG
    lda #$80
    sta BANK_REG
    lda #$0C
    sta CHR_ID_REG

    nop_for 46

    ldy #%10011110
    sty PPU_MASK

    pla
    tay
    pla
    tax
    pla
    rti