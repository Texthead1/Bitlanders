# Bitlanders
![Bitlanders logo](/assets/media/bitlanders_logo_stroke.png)

An in-development NES game based on Skylanders. Created in 6502 asm, C, and custom domain specific assembly language `SkyASM` for external communication.

The NES ROM uses the MMC3 mapper and takes advantage of a theoretical piece of extension hardware - NEspipes - which can interface with USB peripheral devices through its peripheral interface controller (PIC). This additional hardware acts as a bridge between the game and the supported USB peripherals and can be interfaced with by the NES via the extension address line range `4020-4FFF`, which was only ever used by the Famicom Disk System.

This theoretical extension hardware is also under development, currently as a full standalone hardware and NES emulator bundled together, but a lightweight .lua spoofer for the FCEUX NES emulator is also available.

> [!NOTE]
> The .lua simulator will not be able to interface with actual peripheral devices. For full functionality, the custom emulator must be used.

Bitlanders is designed to only optionally require the NESpipes peripheral - the ROM's iNES header declares it as using the regular MMC3 mapper, not a custom one - and so the ROM is purposely designed to be functional and bootable on any MMC3 mapper-compatible emulator/flashcart. No NESpipes-related functionality will evidently be present, so gameplay features may be altered to compensate:
With NESpipes hardware                                                             | Without NESpipes hardware
-----------------------------------------------------------------------------------|--------------------------------
The game plays as intended, with added features made possible due to the hardware. | The game still remains playable, with core functionality intact. NESpipes-specific features are stripped.

> [!WARNING]
> This game is a work-in-progress. It has been tested and verified to function correctly on a PAL-Region NES using an Everdrive N8 Pro, and should work on other region consoles as well. Footage of the game can be found [here](https://youtu.be/S8j5wWYvC68).

## Building
The ROM must be built using the NESpipes Engine, which can be found [here](https://github.com/NESpipes/NESpipes-Engine). Python must also be installed on the machine - Python 3.10+ and above is supported. 3.9 and below will not work.

The game should be built by running the `build.bat` file inside the `scripts` folder that the NESpipes Engine supplies. The following command line arguments should be used:
```
build.bat bitlanders MMC3 game.nes
```

The above arguments will build the game using the MMC3 mapper. It is possible to build the game using the other mappers `NROM` and `CNROM`, but this is only for engine and game development testing.

`--verbose`/`-v` can be supplied as the fourth argument if more debugging information is desired.

The build process will also output a labels file (`bitlanders.lbl`) and compiled 6502 assembly code from C (`main.s`). These files are for debugging purposes only and can largely be ignored.

As a batch file is used for automated building, it can only be used to build the ROM on Windows machines. The ROM should be buildable on other platforms with alternate build processes.