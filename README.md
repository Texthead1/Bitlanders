# Bitlanders
![Bitlanders logo](/assets/media/bitlanders_logo_stroke.png)

An in-development NES game based on Skylanders. Created in 6502 asm, C, and custom assembly `SkyASM` for external communication.

The NES ROM uses an extended MMC3 mapper which houses an additional attached coprocessor - the SkyRetro Peripheral Interface Controller (or SPIC). This additional hardware acts as a bridge between the game and the supported peripherals. This is also under development, currently as a .lua simulator for the FCEUX NES emulator.

> [!NOTE]
> The .lua simulator will not be able to interface with actual peripheral devices. For full functionality, a custom emulator that encapsulates both the NES and SPIC is in the works.

The ROM's iNES header declares it as using the regular MMC3 mapper, not a custom mapper. The result of this is that the ROM remains bootable on any MMC3 mapper-compatible emulator/flashcart. However, no SPIC-related functionality will be present.

Bitlanders specifically is designed to be playable with and without the extended MMC3 mapper.
With the extended mapper                                                       | With the original MMC3 hardware
-------------------------------------------------------------------------------|--------------------------------
The game plays as intended, with added features made possible due to the SPIC. | The game still remains playable, with core functionality intact. SPIC-added features are stripped.

This game is a work-in-progress. It has been tested and verified to function correctly on a PAL-Region NES using an Everdrive N8 Pro, and should work on other region consoles as well. Footage of the game can be found [here](https://youtu.be/S8j5wWYvC68).

## Building
Both a 6502 assembler/compiler/linker toolchain and `python` must be installed on the machine.

The included `build.bat` expects the cc65 toolchain, and to be at the relative path `..\..\cc65\`, but feel free to modify the file path to suit your needs.
- cc65 can be found [here](https://cc65.github.io/)
  - Navigate to the bottom of the page and download the Windows Snapshot release
  - Unpack the archive to the correct directory and make sure the `build.bat` references the cc65 binaries

As a batch file is used for automated building, it can only be used to build the ROM on Windows machines. The ROM should be buildable on other platforms with alternate build processes.

> [!WARNING]
> Due to recent mapper alterations, the current build of the game is extremely unstable. It is not suggested to run the current build. Please build [this snapshot](https://github.com/Texthead1/Bitlanders/tree/b7cfe70102433b66923c19288de18846458658c9) of the repository if you want a gameplay experience.

## Acknowledgements
External 6502 asm and C libraries that have influenced SkyRetro Engine implementations originally provided and maintained by Doug Fraker (a.k.a [nesdoug](https://github.com/nesdoug)). 

Implementations in these original libraries are also subject to be from other authors; all are mentioned inside the respective source files.