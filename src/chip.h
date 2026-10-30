#include <stdint.h>
#include <stdbool.h>

#define FB_WIDTH (64)
#define FB_HEIGHT (32)

#define START_LOC 0x200
#define END_LOC 0xFFF
#define FONTSET_START_ADDRESS 0x50
#define FONTSET_SIZE 80

typedef struct {
  uint8_t registers[16];
  uint8_t memory[4096];
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t program_counter;
  uint8_t stack_pointer;
  uint16_t stack[16];
  uint32_t pixels[FB_HEIGHT * FB_WIDTH];
  uint16_t index;
  bool keypad[16];
} Chip8;

void load_rom(char const* filename, Chip8* chip);

void init_chip(Chip8* chip);

void cycle(Chip8* chip);
