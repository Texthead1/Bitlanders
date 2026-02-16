FT_BASE_ADR		= $0100		;page in RAM, should be $xx00
FT_DPCM_OFF		= $f000		;$c000..$ffc0, 64-byte steps
FT_SFX_STREAMS	= 1			;number of sound effects played at once, 1..4

FT_THREAD       = 1		;undefine if you call sound effects in the same thread as sound update
FT_PAL_SUPPORT	= 1		;undefine to exclude PAL support
FT_NTSC_SUPPORT	= 1		;undefine to exclude NTSC support
FT_DPCM_ENABLE  = 0		;undefine to exclude all DMC code
FT_SFX_ENABLE   = 1		;undefine to exclude all sound effects code

.segment "ZEROPAGE"
    FT_TEMP: 			.res 3