# c8ke

An emulator for the CHIP-8 interpreted language written in C++

> *For more information: [CHIP-8 - Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)*

## Getting Started

To get the project and its vcpkg dependency submodule:

```bash
git clone --recursive https://github.com/dadams05/c8ke.git
```

If you already cloned without submodules, get them with:

```bash
git submodule update --init --recursive
```

### Bootstrap vcpkg

If there’s no `vcpkg.exe` in the `vcpkg/` folder, you need to bootstrap vcpkg so the tool builds itself:

**Windows**

```bash
cd vcpkg
bootstrap-vcpkg.bat
```

**Linux**

```bash
cd vcpkg
./bootstrap-vcpkg.sh
```

### Configure and build

**For IDE users with CMake support:**

- The bundled CMakePresets.json already sets the appropriate toolchain flags.
- Just select the desired preset (e.g. x64 Debug) and CMake will handle dependency installation and building automatically.

**Other environments**

If you’re not using and IDE with CMake support, you must tell CMake to use the vcpkg toolchain so it installs and links dependencies correctly:

```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Features

- Accurate CHIP-8 emulation (timing, sound, instructions)
- Customizable colors for screen and debugger (via ImGui)
- Built-in debugger:
  - Registers, stack, memory viewer
  - Key mapping display
- ROM loader with file dialog support (`.ch8`)
- Beep audio tuning (amount & phase)
- Pause/resume support

## Screenshots

![Screenshot 1](screenshots/screenshot1.png)

![Screenshot 2](screenshots/screenshot2.png)

![Screenshot 3](screenshots/screenshot3.png)

## Credits

- [SDL3](https://github.com/libsdl-org/SDL)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/)

## Extra

There are no ROMs included in this repo, but you can find some here:

- [ROMs for testing the emulator](https://github.com/Timendus/chip8-test-suite)
- [Games and Demos](https://github.com/kripod/chip8-roms)
