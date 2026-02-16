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

.ifdef VRC4
    .include "../mapper/vrc4/startup_vrc4.s"
.endif

.import __STACK_START__, __STACKSIZE__
.import __ROM0_START__,  __ROM0_SIZE__

.import __STARTUP_LOAD__, __STARTUP_RUN__, __STARTUP_SIZE__
.import	__CODE_LOAD__,    __CODE_RUN__,    __CODE_SIZE__
.import	__RODATA_LOAD__,  __RODATA_RUN__,  __RODATA_SIZE__

.import NES_MAPPER, NES_PRG_BANKS, NES_CHR_BANKS, NES_MIRRORING

.importzp _PAD_STATE, _PAD_STATET

OAM_BUF		=$0200

.segment "BSS"
PAL_BUF: .res 32