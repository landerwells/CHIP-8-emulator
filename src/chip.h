#include <stdint.h>

#define FB_WIDTH (64)
#define FB_HEIGHT (32)

#define START_LOC 0x200
#define   END_LOC 0xFFF

typedef struct {
  uint8_t registers[16];
  uint8_t memory[4096];
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t program_counter;
  uint8_t stack_pointer;
  uint16_t stack[16];
  uint32_t pixels[FB_HEIGHT * FB_WIDTH];
  uint16_t opcode;
  // Register used to store memory addresses.
  uint16_t index;
} Chip8;

void load_rom(char const* filename, Chip8* chip);

// Nothing in the Chip8 requires passing around memory?
// This initializes the chip
/* Chip8 init(); */

void cycle(Chip8* chip);
