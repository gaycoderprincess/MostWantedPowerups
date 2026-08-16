# Most Wanted Powerups

A mod for Need for Speed: Most Wanted that gives you powerups from Mario Kart, Re-Volt, Blur and others

### This project has been moved to [Codeberg](https://codeberg.org/gaycoderprincess/MostWantedPowerups) due to GitHub's continued pushing of AI garbage.

<img width="1042" height="671" alt="powerups_thumb" src="https://github.com/user-attachments/assets/3eb7e043-4de5-4a32-bd49-4d13ef48145b" />

## Disclaimer

Due to the current programming landscape, I feel that it's necessary to explicitly state that this project had zero assistance or any other kind of involvement from any sort of "AI agent" and it never will.  
This mod was entirely built by hand, by a human being, and I believe that any project that cannot also claim this about itself is not worth people's time. The only acceptable amount of AI use is zero AI use.

## Installation

- Make sure you have v1.3 of the game, as this is the only version this plugin is compatible with. (exe size of 6029312 bytes, other regions such as the Asian version are not and will never be supported)
- Plop the files into your game folder.
- If you're feeling adventurous, place a ROM of the US version of Super Mario 64 next to the files, renamed to `baserom.us.z64`.
- Enjoy, nya~ :3

## Recommended mods

- [Xbox 360 Stuff Pack](https://nfsmods.xyz/mod/1200)

## Incompatible mods

- Sun Set (fully incompatible)

## Building

Building is done on an Arch Linux system with CLion being used for the build process. 

Before you begin, clone [nya-common](https://github.com/gaycoderprincess/nya-common), [nya-common-nfsmw](https://github.com/gaycoderprincess/nya-common-nfsmw), [CwoeeMenuLib](https://github.com/gaycoderprincess/CwoeeMenuLib) and [CwoeeModelImporter](https://github.com/gaycoderprincess/CwoeeModelImporter) to folders next to this one, so they can be found.

Required packages: `mingw-w64-gcc vcpkg`

To install all dependencies, use:
```console
vcpkg install tomlplusplus:x86-mingw-static curl:x86-mingw-static box3d:x86-mingw-static
```

To install the BASS audio library:

Download the Win32 version from [here](https://www.un4seen.com/bass.html) and extract it somewhere

Once that's done, copy `bass.lib` from the `c` folder into `nya-common/lib32`

You should be able to build the project now in CLion.
