.include "lib/nesdoug/nesdoug.s"

.segment "RODATA"
    .global _demo_input

_demo_input:
    .incbin "../demogame/demo.bin"

.ifdef MMC3
.segment "BANK12"
.endif
music_data:
    .include "../assets/music/molekin_mine.s"

sounds_data:

.segment "CHARS"
.ifdef NROM
    .incbin "../assets/chr/title.chr"
.endif

.ifdef MMC3
    .incbin "../assets/chr/title.chr"
    .incbin "../assets/chr/test.chr"
    .incbin "../assets/chr/characters/23_wreckingball.chr"
    .incbin "../assets/chr/echo.chr"
    .incbin "../assets/chr/blank.chr"
.endif

.ifdef CNROM
    .incbin "../assets/chr/title.chr"
    .incbin "../assets/chr/test.chr"
    .incbin "../assets/chr/characters/23_wreckingball.chr"
    .incbin "../assets/chr/cnrom/padding.chr"
    .incbin "../assets/chr/echo.chr"
    .incbin "../assets/chr/characters/23_wreckingball.chr"
.endif

.segment "CODE"
    .include "lib/famitone2/famitone2.s"
    .include "lib/libnespipes/nespipes.s"