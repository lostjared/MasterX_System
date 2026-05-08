#!/bin/bash

sudo pacman -S --noconfirm --needed base-devel make cmake ninja git gdb sdl2 sdl2_ttf zlib libpng libglvnd
mkdir -p build && cd build
cmake .. -G "Ninja" && ninja && sudo ninja install



