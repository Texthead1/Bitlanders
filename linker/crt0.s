; Startup code for cc65 and Shiru's NES library
; based on code by Groepaz/Hitmen <groepaz@gmx.net>, Ullrich von Bassewitz <uz@cc65.org>

.define SOUND_BANK 12

FT_BASE_ADR		= $0100		;page in RAM, should be $xx00
FT_DPCM_OFF		= $f000		;$c000..$ffc0, 64-byte steps
FT_SFX_STREAMS	= 1			;number of sound effects played at once, 1..4

FT_THREAD       = 1		;undefine if you call sound effects in the same thread as sound update
FT_PAL_SUPPORT	= 1		;undefine to exclude PAL support
FT_NTSC_SUPPORT	= 1		;undefine to exclude NTSC support
FT_DPCM_ENABLE  = 0		;undefine to exclude all DMC code
FT_SFX_ENABLE   = 1		;undefine to exclude all sound effects code

;REMOVED initlib
;this called the CONDES function

    .export _exit,__STARTUP__:absolute=1
	.import push0,popa,popax,_main,zerobss,copydata

; Linker generated symbols
	.import __STACK_START__   ,__STACKSIZE__ ;changed
	.import __ROM0_START__  ,__ROM0_SIZE__
	.import __STARTUP_LOAD__,__STARTUP_RUN__,__STARTUP_SIZE__
	.import	__CODE_LOAD__   ,__CODE_RUN__   ,__CODE_SIZE__
	.import	__RODATA_LOAD__ ,__RODATA_RUN__ ,__RODATA_SIZE__
	.import NES_MAPPER, NES_PRG_BANKS, NES_CHR_BANKS, NES_MIRRORING

	.importzp _PAD_STATE, _PAD_STATET ;added
    .include "zeropage.inc"

PPU_CTRL	=$2000
PPU_MASK	=$2001
PPU_STATUS	=$2002
PPU_OAM_ADDR=$2003
PPU_OAM_DATA=$2004
PPU_SCROLL	=$2005
PPU_ADDR	=$2006
PPU_DATA	=$2007
PPU_OAM_DMA	=$4014
PPU_FRAMECNT=$4017
DMC_FREQ	=$4010
CTRL_PORT1	=$4016
CTRL_PORT2	=$4017

OAM_BUF		=$0200
;PAL_BUF	=$01c0
VRAM_BUF	=$0700

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
FT_TEMP: 			.res 3

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

;nesdoug code requires
VRAM_INDEX:			.res 1
META_PTR:			.res 2
DATA_PTR:			.res 2

.segment "HEADER"
    .byte $4e,$45,$53,$1a
	.byte <NES_PRG_BANKS
	.byte <NES_CHR_BANKS
	.byte <NES_MIRRORING|(<NES_MAPPER<<4)
	.byte <NES_MAPPER&$f0
	.byte 1	; PRG RAM bank for MMC3
	.res 7,0

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

.segment "STARTUP"

start:
_exit:

    sei
	cld
	ldx #$40
	stx CTRL_PORT2
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx DMC_FREQ
    stx PPU_CTRL		;no NMI

	jsr _disable_irq ;disable mmc3 IRQ

initPPU:
    bit PPU_STATUS
@1:
    bit PPU_STATUS
    bpl @1
@2:
    bit PPU_STATUS
    bpl @2

clearPalette:
	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR
	lda #$0f
	ldx #$20
@1:
	sta PPU_DATA
	dex
	bne @1

clearVRAM:
	txa
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

clearRAM:
    txa
@1:
    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @1

; append mmc3 setup here

; MMC3 reset
; todo: move out of crt0, into engine

; set which bank at $8000
; also $c000 fixed to 14 of 15
	lda #0 ; PRG bank zero
	jsr _set_prg_8000

; set which bank at $a000
	lda #13 ; PRG bank 13 of 15
	jsr _set_prg_a000
	
; with CHR invert, set $0000-$03FF
	lda #0
	jsr _set_chr_mode_2

; with CHR invert, set $0400-$07FF
	lda #1
	jsr _set_chr_mode_3

; with CHR invert, set $0800-$0BFF
	lda #2
	jsr _set_chr_mode_4

; with CHR invert, set $0C00-$0FFF
	lda #3
	jsr _set_chr_mode_5

; with CHR invert, set $1000-$17FF
	lda #4
	jsr _set_chr_mode_0

; with CHR invert, set $1800-$1FFF
	lda #6
	jsr _set_chr_mode_1

; set mirroring to vertical, no good reason	
	lda #0
	jsr _set_mirroring

; allow reads and writes to WRAM	
	lda #$80 ;WRAM_ON 0x80
	jsr _set_wram_mode
	
	cli ; allow irq's to happen on the 6502 chip	
		; however, the mmc3 IRQ was disabled above

	lda #4
	jsr _pal_bright
	jsr _pal_clear
	jsr _oam_clear

    jsr	zerobss
	jsr	copydata

    lda #<(__STACK_START__+__STACKSIZE__) ;changed
    sta	sp
    lda	#>(__STACK_START__+__STACKSIZE__)
    sta	sp+1            ; Set argument stack ptr

	lda #%10000000
	sta <PPU_CTRL_VAR
	sta PPU_CTRL		;enable NMI
	lda #%00000110
	sta <PPU_MASK_VAR

waitSync3:
	lda <FRAME_CNT1
@1:
	cmp <FRAME_CNT1
	beq @1

detectNTSC:
	ldx #52				;blargg's code
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

	lda #$fd
	sta <RAND_SEED
	sta <RAND_SEED+1

	lda #0
	sta PPU_SCROLL
	sta PPU_SCROLL

; change to sound bank before FamiTone init

	lda #SOUND_BANK
	jsr _set_prg_8000

	ldx #<music_data
	ldy #>music_data
	lda <NTSC_MODE
	jsr FamiToneInit

	.if(FT_SFX_ENABLE)
	ldx #<sounds_data
	ldy #>sounds_data
	jsr FamiToneSfxInit
	.endif

	lda #$00 ;PRG bank #0 at $8000, back to basic
	jsr _set_prg_8000

	jmp _main			;no parameters

	.include "../engine/mapper/mmc3/mmc3_code.asm"
	.include "../lib/libnes/neslib.s"
	.include "../lib/nesdoug/nesdoug.s"

.segment "RODATA"
	.global _demo_input
_demo_input:
	.incbin "../demogame/demo.bin"

.segment "BANK12"
music_data:
	.include "../assets/music/molekin_mine.s"

sounds_data:

.segment "VECTORS"
    .word nmi	;$fffa vblank nmi
    .word start	;$fffc reset
   	.word irq	;$fffe irq / brk

.segment "CHARS"
	.incbin "../assets/chr/title.chr"
	.incbin "../assets/chr/test.chr"
	.incbin "../assets/chr/characters/23_wreckingball.chr"
	.incbin "../assets/chr/echo.chr"
	.incbin "../assets/chr/blank.chr"

.segment "CODE"
	.include "../lib/famitone2.s"
	.include "../lib/libskyretro/skyretro.s"