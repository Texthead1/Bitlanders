.include "../../engine/common/common.inc"

.importzp _irq_handler_ptr

.importzp _water_scanline_rows_remaining
.importzp _water_scanline_index

.import _water_scanline_offsets

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

    lda #$00
    sta $C000
    sta $C001

    ldx #<(_water_distort_irq)
    stx _irq_handler_ptr
    ldx #>(_water_distort_irq)
    stx _irq_handler_ptr+1

    sta $E001

    pla
    tay
    pla
    tax
    pla
    rti

_water_distort_irq:
    pha
    txa
    pha

    lda #$00
    sta $E000

    nop_for 31
    bit $00

    ldx _water_scanline_index
    lda _water_scanline_offsets, x
    clc
    adc _cam_latch_x
    sta PPU_SCROLL
    lda _cam_latch_y
    sta PPU_SCROLL

    inx
    txa
    and #$1F
    sta _water_scanline_index

    dec _water_scanline_rows_remaining
    beq @exit

    lda #$00
    sta $C000
    sta $C001
    sta $E001

@exit:
    pla
    tax
    pla
    rti