# Udon Lua

Lua interpreter in VRChat

## How to build Lua to UdonSharp

### Tested environment

* Ubuntu 22.04 (on WSL 2)
* CMake 3.25.1
* WASI SDK 22.0
* Binaryen 117
* wasm2usharp 0.1.0

### Install dependencies

```bash
# Install CMake
sudo apt update && sudo apt upgrade
sudo apt install cmake

# Get WASI SDK
wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-22/wasi-sdk-22.0-linux.tar.gz
tar xf wasi-sdk-22.0-linux.tar.gz

# Get Binaryen
wget https://github.com/WebAssembly/binaryen/releases/download/version_117/binaryen-version_117-x86_64-linux.tar.gz
tar xf binaryen-version_117-x86_64-linux.tar.gz

# Get wasm2usharp
wget https://github.com/raii-x/wasm2usharp/releases/download/v0.1.0/wasm2usharp-x86_64-unknown-linux-gnu.tar.gz
mkdir wasm2usharp
tar xf wasm2usharp-x86_64-unknown-linux-gnu.tar.gz -C wasm2usharp

# Set environment variables
export WASI_SDK_PATH=`pwd`/wasi-sdk-22.0
export PATH=`pwd`/binaryen-version_117/bin:$PATH
export PATH=`pwd`/wasm2usharp:$PATH
```

### Build Lua

```bash
# Current directory is the root of this repository
cmake -S lua -B lua/build
cmake --build lua/build
wasm2usharp lua/build/lua.wasm -o Assets/Scripts/Lua.cs
```

## Changes from original Lua

The Lua code in this repository is based on Lua 5.4.6.
Some functions in Lua standard libraries that are not supported in WASI are disabled.
Ruby's implementation for Asyncify ([lua/rb_wasm](lua/rb_wasm)) is used to use setjmp/longjmp and split long processing to multiple frames (in [lvm.c](lua/src/lvm.c#L1136)).
Functions for Udon are implemented in [runtime.c](lua/src/runtime.c).
