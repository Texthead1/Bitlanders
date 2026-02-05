.import _water_palette
.import irq_handler_ptr

.segment "CODE"
.export _water_irq

_water_irq:
    pha
    txa
    pha
    tya
    pha

    sta $E000

    lda #%00000000
    sta $2001

    lda #$3F
    sta $2006
    lda #$00
    sta $2006

    ldx #16
@loop:
    lda _water_palette
    sta $2007
    dex
    bpl @loop

    lda #%00011110
    sta $2001

    pla
    tay
    pla
    tax
    pla
    rti