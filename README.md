<img src="./art/livearea/icon0-fs8.png" align="left" alt="The application icon."/> 

# vitaTrak
Playstation Vita homebrew for playing [Tracker music](https://en.wikipedia.org/wiki/Music_tracker) files.

Made with [CLion](https://www.jetbrains.com/clion/), the souls of the damned, and [VitaSDK](https://vitasdk.org/).

## Features
- Supports many traditional tracker formats, including:
  - `MOD`,`XM`,`S3M`,`IT`,`MPTM`,`MIDI`, and many more!
- User-friendly UI
- Native [ModArchive](https://modarchive.org/) support to search and download music

## Building
1. Install [VitaSDK](https://vitasdk.org/) and configure it
2. (optional, but recommended) Install [CLion](https://www.jetbrains.com/clion/) and [Configure it for VitaSDK](https://gist.github.com/samdisk11/44cd9e82ea68c07ccb84d497df787634)
3. Clone [libOpenMPT](https://github.com/vitasdk/packages/tree/master/libopenmpt) and build without support for MPG123 (Because OpenMPT calls 64-bit MPG123 instructions that have not been ported yet, see issue #1) using `--without-mpg123`
   - Alternatively, you could download our pre-built `libOpenMPT.a` that is located in the `.github` folder.
4. Place your built `libOpenMPT.a` file in your `VITASDK/arm-vita-eabi/lib/` folder, replacing the original one with your newly built one.
5. Clone this repository
6. Open the project in CLion (or your IDE of choice)
7. Configure `headers/config.h` to your liking
8. Build the project and run it on your Vita!

## Dependencies
This project uses the following libraries:
- [libOpenMPT_vita (Included with vitaSDK)](https://github.com/vitasdk/packages/tree/master/libopenmpt)
- [libImGui_vita2D (Included in source)](https://github.com/cy33hc/imgui-vita2d)

## Donations
If you like my work, please consider contributing to the code or donate some [Monero](https://getmonero.org)
to `45w49S4pDdu52gqks8NkKZjEkxu4cFS7sQjqXL6F3kf3aaT92xW1ScGFKshgonCdRxT3mEVpSdBHiX6A6xx6PQye32T99o1`
