# Nandy2024

## Build

### OSX

```sh
export SDKROOT=$(xcrun --show-sdk-path --sdk macosx)

export PATH="/opt/homebrew/Cellar/gcc/13.2.0/bin:$PATH"
export PATH="/opt/homebrew/Cellar/llvm/17.0.6_1/bin:$PATH"

alias gcc="/opt/homebrew/Cellar/gcc/13.2.0/bin/gcc-13"
alias g++="/opt/homebrew/Cellar/gcc/13.2.0/bin/g++-13"

alias clang="/opt/homebrew/Cellar/llvm/17.0.6_1/bin/clang"
alias clang++="/opt/homebrew/Cellar/llvm/17.0.6_1/bin/clang++"

export CC=clang
export CXX=clang++

mkdir build
cmake -S . -B build
cmake --build build -j16
```
