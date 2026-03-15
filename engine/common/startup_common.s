.include "header.inc"
.include "zeropage.inc"
.include "common.inc"

.ifdef NROM
    .include "../mapper/nrom/startup_nrom.s"
.endif

.ifdef CNROM
    .include "../mapper/cnrom/startup_cnrom.s"
.endif

.ifdef MMC3
    .include "../mapper/mmc3/startup_mmc3.s"
.endif

.import __STACK_START__, __STACKSIZE__
.import __ROM0_START__,  __ROM0_SIZE__

.import __STARTUP_LOAD__, __STARTUP_RUN__, __STARTUP_SIZE__
.import	__CODE_LOAD__,    __CODE_RUN__,    __CODE_SIZE__
.import	__RODATA_LOAD__,  __RODATA_RUN__,  __RODATA_SIZE__

.import NES_MAPPER, NES_PRG_BANKS, NES_CHR_BANKS, NES_MIRRORING

.importzp _PAD_STATE, _PAD_STATET

.segment "VECTORS"
    .word nmi
    .word start
    .word irq

OAM_BUF		=$0200

.segment "BSS"
PAL_BUF: .res 32

.segment "ZEROPAGE"
NTSC_MODE: 			.res 1
FRAME_CNT1: 		.res 1
FRAME_CNT2: 		.res 1
VRAM_UPDATE: 		.res 1
NAME_UPD_ADR: 		.res 2
NAME_UPD_ENABLE: 	.res 1
PAL_UPDATE: 		.res 1
PAL_BG_PTR: 		.res 2
PAL_SPR_PTR: 		.res 2
SCROLL_X: 			.res 1
SCROLL_Y: 			.res 1
SCROLL_X1: 			.res 1
SCROLL_Y1: 			.res 1
PAD_STATE: 			.res 2		;one byte per controller
PAD_STATEP: 		.res 2
PAD_STATET: 		.res 2
PPU_CTRL_VAR: 		.res 1
PPU_CTRL_VAR1: 		.res 1
PPU_MASK_VAR: 		.res 1
RAND_SEED: 			.res 2

TEMP: 				.res 11
SPRID:				.res 1

PAD_BUF		=TEMP+1

PTR			=TEMP	;word
LEN			=TEMP+2	;word
NEXTSPR		=TEMP+4
SCRX		=TEMP+5
SCRY		=TEMP+6
SRC			=TEMP+7	;word
DST			=TEMP+9	;word

RLE_LOW		=TEMP
RLE_HIGH	=TEMP+1
RLE_TAG		=TEMP+2
RLE_BYTE	=TEMP+3

VRAM_INDEX:			.res 1
META_PTR:			.res 2
DATA_PTR:			.res 2

.include "../../game/include.inc"
.include "../../lib/libnes/neslib.s"

; implementation help from: https://nesdev.org/wiki/init_code
start:
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
    stx DMC_FREQ ; disable DMC IRQs

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
