# CHIP-8 interpreter in C 

This is my implementation of hand-written C code for the CHIP-8 interpreter.

I used [Sokol](https://github.com/floooh/sokol), a C header library for the
window, and input detection.

## Usage

Currently MacOS only.

Ensure you have `make` installed. The binary only takes two command line
arguments, the first is for the ROM file, and the second is for the desired
hertz for the virtual machine to run at.

``` sh
make
./chip ROM frequency
```

