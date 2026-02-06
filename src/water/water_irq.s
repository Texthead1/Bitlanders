.import _water_palette
.import irq_handler_ptr
.importzp _cam_latch_x
.importzp _cam_latch_y

.segment "CODE"
.export _water_irq

_water_irq:
    pha
    txa
    pha
    tya
    pha

    lda #$00
    ldx #$3F
    ldy #$01

    sta $E000

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    sta $2001 ; rendering off
    stx $2006 ; palette
    sta $2006
    sty $2007 ; set palette color

    sta $8000
    lda #$12
    sta $8001
    ldy #%00101110
    sty $2001

    ldy _cam_latch_x
    sty $2005
    ldy _cam_latch_y
    sty $2005

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    lda #$00
    sta $8000
    lda #$08
    sta $8001
    lda #$80
    sta $8000
    lda #$0C
    sta $8001

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    ldy #%00111110
    sty $2001

    pla
    tay
    pla
    tax
    pla
    rti