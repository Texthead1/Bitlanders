.include "header.inc"
.include "zeropage.inc"
.include "addresses.inc"
.include "macros.inc"

.ifdef NROM
    .include "../mapper/nrom/startup_nrom.s"
.endif

.ifdef CNROM
    .include "../mapper/cnrom/startup_cnrom.s"
.endif

.ifdef MMC3
    .include "../mapper/mmc3/startup_mmc3.s"
.endif

.export _exit, __STARTUP__:absolute=1

; these are defined by cc65: https://github.com/cc65/cc65/blob/master/libsrc/
.import   push0, popa, popax, _main, zerobss, copydata

; and these by the linker
.import   __STACK_START__, __STACKSIZE__
.import   __ROM0_START__,  __ROM0_SIZE__

.import   __STARTUP_LOAD__, __STARTUP_RUN__, __STARTUP_SIZE__
.import   __CODE_LOAD__,    __CODE_RUN__,    __CODE_SIZE__
.import   __RODATA_LOAD__,  __RODATA_RUN__,  __RODATA_SIZE__

.import   NES_MAPPER, NES_PRG_BANKS, NES_CHR_BANKS, NES_MIRRORING, NES_PRG_RAM_BANKS


OAM_BUF  = $0200
VRAM_BUF = $0700

.segment "BSS"
PAL_BUF: .res 32

.segment "ZEROPAGE"
NTSC_MODE:          .res 1
FRAME_CNT1:         .res 1
FRAME_CNT2:         .res 1
VRAM_UPDATE:        .res 1
NAME_UPD_ADR:       .res 2
NAME_UPD_ENABLE:    .res 1
PAL_UPDATE:         .res 1
PAL_BG_PTR:         .res 2
PAL_SPR_PTR:        .res 2
SCROLL_X:           .res 1
SCROLL_Y:           .res 1
SCROLL_X1:          .res 1
SCROLL_Y1:          .res 1
PAD_STATE:          .res 2      ;one byte per controller
PAD_STATEP:         .res 2
PAD_STATET:         .res 2
PPU_CTRL_VAR:       .res 1
PPU_CTRL_VAR1:      .res 1
PPU_MASK_VAR:       .res 1
RAND_SEED:          .res 2

TEMP:               .res 11
SPRID:              .res 1

PAD_BUF     =TEMP+1

PTR         =TEMP   ;word
LEN         =TEMP+2 ;word
NEXTSPR     =TEMP+4
SCRX        =TEMP+5
SCRY        =TEMP+6
SRC         =TEMP+7 ;word
DST         =TEMP+9 ;word

RLE_LOW     =TEMP
RLE_HIGH    =TEMP+1
RLE_TAG     =TEMP+2
RLE_BYTE    =TEMP+3

VRAM_INDEX:         .res 1
META_PTR:           .res 2
DATA_PTR:           .res 2

.include "../../game/include.inc"
.include "../../lib/libnes/neslib.s"

.segment "STARTUP"

; implementation help from: https://nesdev.org/wiki/init_code
; and: https://github.com/cc65/cc65/blob/master/libsrc/nes/crt0.s
start:
_exit:
    sei
    cld

    ; disable APU for IRQs
    ldx #$40
    stx CTRL_PORT2

    ; stack pointer reset
    ldx #$FF
    txs
    inx
    stx PPU_CTRL ; disable NMI
    stx PPU_MASK ; disable rendering
    stx DMC_FREQ ; disable DMC 

    jsr _init_mapper_immediate

    ; vblank startup
    bit PPU_STATUS

@vb1:
    bit PPU_STATUS
    bpl @vb1

    ; X is 0, we'll use the accumulator to 0-out RAM
    txa

@clearRAM:
    sta $000, x
    sta $100, x
    sta $200, x
    sta $300, x
    sta $400, x
    sta $500, x
    sta $600, x
    sta $700, x
    inx
    bne @clearRAM

@vb2:
    bit PPU_STATUS
    bpl @vb2

clearPalette:
    lda #$3F
    sta PPU_ADDR
    stx PPU_ADDR
    lda #$0F
    ldx #$20
@1:
    sta PPU_DATA
    dex
    bne @1

    txa

clearVRAM:
    ldy #$20
    sty PPU_ADDR
    sta PPU_ADDR
    ldy #$10
@1:
    sta PPU_DATA
    inx
    bne @1
    dey
    bne @1

    jsr _init_mapper

    ; set virtual palette brightness to normal
    lda #$04
    jsr _pal_bright

    ; clear palette and oam
    jsr _pal_clear
    jsr _oam_clear

    jsr zerobss
    jsr copydata

    ; set up stack
    lda #<(__STACK_START__+__STACKSIZE__)
    sta sp
    lda #>(__STACK_START__+__STACKSIZE__)
    sta sp+1

    ; turn on nmi
    lda #%10000000
    sta <PPU_CTRL_VAR
    sta PPU_CTRL

    ; turn on bg/spr rendering (don't push to ppu mask yet)
    lda #%00000110
    sta <PPU_MASK_VAR

wait:
    lda <FRAME_CNT1
@1:
    cmp<FRAME_CNT1
    beq @1

detectNTSC:
    ldx #52
    ldy #24
@1:
    dex
    bne @1
    dey
    bne @1

    lda PPU_STATUS
    and #$80
    sta <NTSC_MODE

    jsr _ppu_off

    lda #0
    ldx #0
    jsr _set_vram_update

    lda #$FD
    sta <RAND_SEED
    sta <RAND_SEED+1

    lda #0
    sta PPU_SCROLL
    sta PPU_SCROLL

    jsr _init_famitone
    cli

    jmp _main

.segment "VECTORS"
    .word nmi
    .word start
    .word irq