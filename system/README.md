# MasterX System

## How to Compile

### Step 1: Compile and Install `libcxx_scan` and `ETL` (Optional)

If you wish to use `ETL` (Extended Template Library), follow these instructions to compile and install the necessary dependencies:

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


   
