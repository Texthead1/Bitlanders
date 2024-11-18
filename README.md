# Bitlanders
A personal, in-development NES game based on Skylanders, made in 6502 ASM and C (utilizing [nesdoug](https://github.com/nesdoug)'s libraries).

There are also additional plans to create a custom emulator for this game to add pseudo-Portal support, but the ROM will remain fully playable outside of this as well.

This game is a work-in-progress. It is also currently untested on real NES hardware and may potentially misbehave in some emulators, but functions as intended in FCEUX.

## Building
You must have a 6502 assembly compiler installed onto the machine - the included compile.bat expects cc65. The included `compile.bat` expects the cc65 directory to exist at the relative path `..\..\cc65\`, but feel free to modify the file path to suit your needs.
* cc65 can be found [here](https://cc65.github.io/)
  * Navigate to the bottom of the page and download the Windows Snapshot release
  * Unpack the archive to the correct directory and make sure the `compile.bat` references the cc65 binaries
