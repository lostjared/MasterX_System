
# libetl Installation Guide

This document explains how to install the `libetl` project using CMake. The project contains two main components: `libetl` and `libetl_sdl`, which are static libraries. It also installs related `.e` files to the appropriate locations.

## Prerequisites

Before installing the project, ensure that the following prerequisites are met:

### Required Tools

- GCC (or any C compiler)
- CMake (version 3.10 or higher)
- SDL2 (including the `sdl2-config` tool)
- GNU `ar` (for creating static libraries)
- Make (optional, if using CMake’s default generator)

### Installation Paths

By default, the libraries and headers will be installed in `/usr/local`. If you want to install them in a custom location, you will need to modify the `CMakeLists.txt` file or pass the `ETL_PATH` as an option during configuration.

## Installation Steps

Follow the steps below to install the project.

### Configure the Build System

Use CMake to generate the build system. First, create a `build` directory and navigate into it:

```bash
mkdir build
cd build
```

Then run the CMake configuration command:

```bash
cmake ..
```

By default, the project will be installed in `/usr/local`. If you want to specify a different installation path for `ETL_PATH`, you can provide it using the following command:

```bash
cmake -DETLPATH=/your/custom/path ..
```

### Build the Project

Once CMake has configured the project, build it by running:

```bash
cmake --build .
```

This command compiles the source files (`libetl.c` and `sdl.c`) and generates two static libraries: `libetl.a` and `libetl_sdl.a`.

### Install the Project

After building the project, install it using the following command:

```bash
sudo cmake --install .
```

This will copy the following files to the designated paths:
- `libetl.a` and `libetl_sdl.a` to `/usr/local/lib` (or the custom `ETL_PATH` you specified).
- Any `.e` files to `/usr/local/include/libetl/` (or the custom path).

### Cleaning Up

To clean up the build directory and object files, you can run the `cleanup` target:

```bash
cmake --build . --target cleanup
```

Alternatively, you can remove the entire `build` directory:

```bash
cd ..
rm -rf build
```

## Customizing Installation Path

To customize the installation path, set the `ETL_PATH` variable during the configuration step. For example:

```bash
cmake -DETLPATH=/my/custom/etl/path ..
```

The libraries and headers will be installed in the specified directory.

## Troubleshooting

- If you encounter any missing package errors, ensure that SDL2 is installed on your system and that the `sdl2-config` tool is available in your path.
- If CMake cannot find SDL2, you may need to manually specify the path to SDL2 using `-DSDL2_DIR=/path/to/sdl2`.

---

By following these steps, you should be able to successfully build and install `libetl` and `libetl_sdl`. If you encounter any issues, refer to the CMake output for more details or consult the project's documentation for further assistance.
