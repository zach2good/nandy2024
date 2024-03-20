# 🤖 nandy2024
NAND-level circuit simulation

## Introduction

I _really_ like [nand2tetris](https://www.nand2tetris.org/) and it's accompanying book [The Elements of Computing Systems, by Noam Nisan and Shimon Schocken](https://www.amazon.com/Elements-Computing-Systems-Building-Principles/dp/0262640686/ref=ed_oe_p).
One of my only criticisms was the lack of ability to zoom in and inspect the base NAND gates from any level of abstraction, those abstractions being enforced by successively high-level simulators.

My goal is to create a simulator that will allow you to complete the course, using only drag-and-drop NAND gates and simple supporting nodes, labels, pre-made circuits, etc.

This is a continuation of my [first attempt at a NAND simulator](https://github.com/zach2good/nandy).

## TODO

- Component Deletion
  - Breaking of connections.
  - All references and links, including in the UI, should be cleared out when a component is removed.
  - If a group, all in that group should be safely deleted.
  - ID reuse isn't important (for now).
  - Deletion clears the current selection.
- Component Selection
  - Mouse-over and Selection should be split. Single-component selection is the same as any other selection.
  - Track components that were selected. Move them around once selected. This could be a pseudo/temporary group?
  - Option to turn selection into a group.
  - Option to delete all components in a selection.
  - If all of selection is NODEs, have a button (or a button for each connection "direction") that will try to mass-connect them. Will need an undo button because of this.
  - If you are linking a collection of nodes left-to-right, it'll split them down the middle, and then try to link them in order:
  ```
  (Left)   (Right)
  A0    ->  A1
  B0    ->  B1
  C0    ->  C1
  ```
- Component Grouping
  - This also enables the blocking of connecting nodes of the same gate together.
  - If these are arbitrary groups then this can be the model of compound gates like AND/OR out of NANDs etc.
  - Option to analyze input/outputs of groups and skip full simulation for a performance increase.
    - This sort of thing could be used to generate truth tables?
  - Save groups as their own Circuit files, so they can be loaded and reproduced
- User Workflow
  - You should be able to build each component from NANDs
  - You should be able to easily move components around, with grid-snapping, or without (hold shift while dragging to disable snapping?)
  - You should be able to select them, and group them, and save that group
  - You should be able to select and duplicate groups of components
  - You should be able to mass-connect large areas of nodes together
  - You should be able to build more advanced concepts with the exact groups you've already built earlier on - rather than all the groups being pre-made for you. Sense of user achievement.
  - You should be able to work through the whole book, gates up to ALU up to CPU and writing program, and zoom in/out on the NAND gates at any stage.

## Controls

- Drag n' Drop component from the left panel onto the canvas with L.Mouse.
- Move around on canvas by dragging Shift+L.Mouse.
- Make selections of components by dragging L.Mouse.
- Connect nodes by dragging L.Mouse between them.
- Connect nodes in a selection by pressing C with an active selection.
- Move components with L.Mouse.
- Press R with components selected or with a component under your mouse to rotate it 90 degrees.

## Components

All of these should be pre-made and usable from the left panel.
They should come out as a pre-made grouping of NANDs, or other sub-components, that you can only modify if you break their grouping.

```
Chapter 1
    NAND
    NODE (Unofficial)
    CLK (Unofficial)
    LABEL (Unofficial)
    NOT
    AND
    OR
    XOR
    MUX
    DMUX
    NOT16
    AND16
    OR16
    MUX16
    OR8WAY
    MUX4WAY16
    MUX8WAY16
    DMUX4WAY
    DMUX8WAY

Chapter 2
    HALFADDER
    FULLADDER
    ADD16
    INC16
    ALU
    FULLALU

Chapter 3
     DFF
     BIT
     REGISTER
     RAM8
     RAM64
     RAM512
     RAM4K
     RAM16K
     PC

Chapter 5
     MEMORY
     CPU
     COMPUTER

Other
     AREGISTER
     DREGISTER
     KEYBOARD
     SCREEN
     ROM32K
```

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

# Note: Doesn't work with AppleClang because of constexpr features
export CC=clang
export CXX=clang++

mkdir build
cmake -S . -B build
cmake --build build -j16
```

### Windows

// TODO

### Linux

// TODO

### WebAssembly (Emscripten)

// TODO: Deploy and host in GitHub Pages

## Performance

// TODO

## Testing & Correctness

// TODO

## License

// TODO
