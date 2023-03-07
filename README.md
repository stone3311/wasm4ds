# WASM4DS

WASM4DS is a project that aims to run games made for the [WASM-4 fantasy console](https://github.com/aduros/wasm4) on the NDS.
It is based on the [WASM-4 native runtime](https://github.com/aduros/wasm4/tree/main/runtimes/native/src), the [WASM3 interpreter](https://github.com/wasm3/wasm3) and [libnds](https://github.com/devkitPro/libnds).

## Features and limitations

- [x] Framebuffer
- [x] Gamepad
- [x] Loading ROMs from a filesystem
- [ ] Audio
- [ ] Save states
- [ ] Netplay

### Notes
- WASM4DS is technically able to run every game as it uses the standard WASM-4 runtime. However, CPU performance on the NDS is limited and more complex games will not run at 60Hz.
- Audio is not enabled at the moment due to the lack of an FPU. A future implementation would have to adapt the DSP code to use fixed point calculations.
- Game ROMs can be loaded from a FAT filesystem on the cartridge. Note that this will not work on most emulators.

## Compiling

You need to have devkitARM installed in order to compile WASM4DS. The `compile.sh` script will compile WASM3 and the WASM-4 runtime and build the .nds rom file. It also fetches the platformer test game and converts it to a C header file.

Alternatively, the devkitPro Docker image can be used to build the ROM:

```
docker pull devkitpro/devkitarm:latest
docker run -it -v $(pwd):/wasm4ds devkitpro/devkitarm bash -c 'apt update; apt install -y xxd; cd /wasm4ds; ./compile.sh'
```

A build is also available on the [GitHub actions page](https://github.com/stone3311/wasm4ds/actions).

## Usage

On a real console, WASM4DS can load games from a FAT filesystem. The `.wasm` cartridges have to be placed in a `/wasm` directory at the root of the filesystem.

The filesystem is usually not available when using an emulator. In this case, WASM4DS has to be recompiled with a different ROM. The [platformer test game](https://github.com/aduros/wasm4/tree/main/examples/platformer-test) is included by default. Edit the relevant line in `compile.sh` to change this.

## Screenshot

![Screenshot of WASM4DS running a game](https://github.com/stone3311/wasm4ds/blob/main/.github/img/screenshot.png?raw=true)

## License

WASM4DS uses the MIT license.

