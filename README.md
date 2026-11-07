# CHIP-8-emulator
A modern, hand-written CHIP-8 emulator written in C. (And yes, I know the
CHIP-8 is an interpreter.)

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
## Images from Timendus' Test Suite

[](assets/1-chip-8-logo.png)

## References

- [Timendus' CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite)
  This was the most useful resource I found. It has tests for the modern
  behavior of the CHIP-8 and walks through which instructions are needed for
  which step.
- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
  This guide was useful for a no-code walkthrough of all the instructions, and
  a great starting point for anyone looking to read more without getting
  overwhelmed by technical details.
- [Making a Chip8 Emulator - Austin Gelber](https://www.austingelber.com/posts/emulator-chip-8/)
  I somehow found this after I had mostly completed my emulator, but it is
  incredibly detailed and well written.
- [Building a CHIP-8 Emulator \[C++\] - Austin Morlan](https://austinmorlan.com/posts/chip8_emulator/)
  I wish I could recommend this article more highly, but it does have some
  slight innaccuracies that make a CHIP-8 journey more complicated. It was
  where I got started, so it was worth putting here.

