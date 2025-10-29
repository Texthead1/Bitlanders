# Bitlanders
![Bitlanders logo](/bitlanders_logo_stroke.png)

A personal, in-development NES game based on Skylanders, made in 6502 ASM and C (utilizing [nesdoug](https://github.com/nesdoug)'s libraries).

Plans to create a custom emulator for this game to add pseudo-Portal support are in the works, but the ROM will remain fully playable outside of this as well.

This game is a work in progress. It has been tested and verified to function correctly on a PAL-Region NES using an Everdrive N8 Pro, and should work on other region consoles as well. Footage of the game can be found [here](https://youtu.be/S8j5wWYvC68?si=Nb9N-eKjDlO0_pPH).

## Building
You must have a 6502 assembly compiler installed on the machine. The included `compile.bat` expects the cc65 compiler, and to be at the relative path `..\..\cc65\`, but feel free to modify the file path to suit your needs.
* cc65 can be found [here](https://cc65.github.io/)
  * Navigate to the bottom of the page and download the Windows Snapshot release
  * Unpack the archive to the correct directory and make sure the `compile.bat` references the cc65 binaries
