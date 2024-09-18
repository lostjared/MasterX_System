# MasterX System

![image](https://github.com/user-attachments/assets/f12969a1-a434-4e5d-acf8-5d2c32d50b7c)

## Notes ##
There are two options for compiling this on Windows
If you are going to compile this and run it within WSL2 export this variable before running.

```bash
export LIBGL_ALWAYS_SOFTWARE=1
```

 Otherwise if Using a native Windows EXE with a pipe to WSL2 this step is unnecessary
 

it will then  use the default software-based render driver for WSL2 on Ubutnu 24.04 LTS, amd Arch. On Arch you have to install and configure an X server. 

## How to Compile

### Step 1: Compile and Install `libcxx_scan` and `ETL` (Optional)

If you wish to use `ETL` (Easy Toy Langauge), follow these instructions to compile and install the necessary dependencies:

1. **Enter the compiler directory:**

    ```bash
    cd MasterX_System/compiler
    mkdir build && cd build
    cmake ..
    make -j4
    sudo make install
    cd ..
    ```

2. **Install ETL:**

    Navigate to the ETL directory and repeat the process:

    ```bash
    cd ETL
    mkdir build && cd build
    cmake ..
    make -j4
    sudo make install
    cd ..
    ```

### Step 2: Compile the MasterX System

This step requires that you installed SDL2, SDL2_ttf, libpng, zlib
if on Linux or WSL2 use the package manager for the distro you have using examples are pacman, or apt.
You may have to set the paths for CMake to see it, it will tell you if there is an issue.

1. **Enter the system directory:**

    ```bash
    cd MasterX_System/system
    mkdir build && cd build
    cmake ..
    make -j4
    ```

2. **Run the application with assets:**

    ```bash
    ./MasterX --path ../assets
    ```

### Step 3: Compile the WebAssembly Version with ATS

1. **Compile and install `zlib` and `libpng`:**

    Follow the standard installation process for `zlib` and `libpng`.

   In my version of the Makefile I use LIBS_PATH=/home/jared/emscripten-libs
   
3. **Edit the `Makefile.em`:**

    Set the path to the directories where you installed `zlib` and `libpng`.

4. **Issue the compilation command:**

    ```bash
    make -f Makefile.em -j4
    ```

5. **Test the WebAssembly build:**

    Serve the application locally using Python’s HTTP server:

    ```bash
    python3 -m http.server 3000
    ```

    Open your browser and point it to `localhost:3000` to access `MasterX.html`.


   
