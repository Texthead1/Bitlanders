.include "../lib/nesdoug/nesdoug.s"

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
    .incbin "../assets/chr/title.chr"
    .incbin "../assets/chr/test.chr"
    .incbin "../assets/chr/characters/23_wreckingball.chr"
    .incbin "../assets/chr/echo.chr"
    .incbin "../assets/chr/blank.chr"

.segment "CODE"
    .include "../lib/famitone2/famitone2.s"
    .include "../lib/libskyretro/skyretro.s"