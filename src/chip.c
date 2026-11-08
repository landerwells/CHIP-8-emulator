#include "chip.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void op_00E0(Chip8* chip);
static void op_00EE(Chip8* chip);

static void op_8xy0(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy1(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy2(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy3(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy4(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy5(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy6(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xy7(Chip8* chip, uint16_t x, uint16_t y);
static void op_8xyE(Chip8* chip, uint16_t x, uint16_t y);

static void op_ExA1(Chip8* chip, uint16_t x);
static void op_Ex9E(Chip8* chip, uint16_t x);

static void op_Fx07(Chip8* chip, uint16_t x);
static void op_Fx0A(Chip8* chip, uint16_t x);
static void op_Fx15(Chip8* chip, uint16_t x);
static void op_Fx18(Chip8* chip, uint16_t x);
static void op_Fx1E(Chip8* chip, uint16_t x);
static void op_Fx29(Chip8* chip, uint16_t x);
static void op_Fx33(Chip8* chip, uint16_t x);
static void op_Fx55(Chip8* chip, uint16_t x);
static void op_Fx65(Chip8* chip, uint16_t x);

static uint16_t opcode = 0;
static uint16_t op = 0;
static uint16_t x = 0;
static uint16_t y = 0;
static uint16_t n = 0;
static uint16_t kk = 0;
static uint16_t nnn = 0;

void (*table[0xF + 1]) ();
void (*table0[0xE + 1]) (Chip8*);
void (*tableE[0xE + 1]) (Chip8*, uint16_t);
void (*table8[0xE + 1]) (Chip8*, uint16_t, uint16_t);
void (*tableF[0x65 + 1]) (Chip8*, uint16_t);

void load_rom(const char* filename, Chip8* chip) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  uint16_t program_size = END_LOC - START_LOC;
  size_t bytesRead = fread(&chip->memory[START_LOC], sizeof(unsigned char), program_size, file);
  if (bytesRead < 0) {
    perror("Error reading file");
    exit(EXIT_FAILURE);
  }

  fclose(file);
}
void init_chip(Chip8* chip) {
  chip->program_counter = START_LOC;
  chip->waiting = -1;

  uint8_t fontset[FONTSET_SIZE] =
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  for (uint8_t i = 0; i < FONTSET_SIZE; i++) {
    chip->memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }

  table8[0x0] = &op_8xy0;
  table8[0x1] = &op_8xy1;
  table8[0x2] = &op_8xy2;
  table8[0x3] = &op_8xy3;
  table8[0x4] = &op_8xy4;
  table8[0x5] = &op_8xy5;
  table8[0x6] = &op_8xy6;
  table8[0x7] = &op_8xy7;
  table8[0xE] = &op_8xyE;

  table0[0x0] = &op_00E0;
  table0[0xE] = &op_00EE;

  tableE[0x1] = &op_ExA1;
  tableE[0xE] = &op_Ex9E;

  tableF[0x07] = &op_Fx07;
  tableF[0x0A] = &op_Fx0A;
  tableF[0x15] = &op_Fx15;
  tableF[0x18] = &op_Fx18;
  tableF[0x1E] = &op_Fx1E;
  tableF[0x29] = &op_Fx29;
  tableF[0x33] = &op_Fx33;
  tableF[0x55] = &op_Fx55;
  tableF[0x65] = &op_Fx65;
}

static void op_ExA1(Chip8* chip, uint16_t x) {
  if (!chip->keypad[chip->registers[x]])
  {
    chip->program_counter += 2;
  }

}

static void op_Ex9E(Chip8* chip, uint16_t x) {
  if (chip->keypad[chip->registers[x]])
  {
    chip->program_counter += 2;
  }
}

static void op_Fx07(Chip8* chip, uint16_t x) {
  chip->registers[x] = chip->delay_timer;
}

static void op_Fx0A(Chip8* chip, uint16_t x) {
  if (chip->keypad[x] == 0 && chip->waiting != -1) {
    chip->registers[x] = chip->waiting;
  }

  for (uint8_t i = 0; i <= 0xF; i++) {
    if (chip->keypad[i])
    {
      chip->waiting = i;
      break;
    }
  }

  if (chip->waiting == -1) 
  {
    chip->program_counter -= 2;
  }
}

static void op_Fx15(Chip8* chip, uint16_t x) {
  chip->sound_timer = chip->registers[x];
}


static void op_Fx18(Chip8* chip, uint16_t x) {
  chip->delay_timer = chip->registers[x];
}

static void op_Fx1E(Chip8* chip, uint16_t x) {
  chip->index += chip->registers[x];
}

static void op_Fx29(Chip8* chip, uint16_t x) {
  chip->index = FONTSET_START_ADDRESS + (5 * chip->registers[x]);

}

static void op_Fx33(Chip8* chip, uint16_t x) {
  uint8_t value = chip->registers[x];

  chip->memory[chip->index + 2] = value % 10;
  value /= 10;

  chip->memory[chip->index + 1] = value % 10;
  value /= 10;

  chip->memory[chip->index] = value % 10;
}

// Fx55: Stores registers V0 through Vx in memory starting at location I
static void op_Fx55(Chip8* chip, uint16_t x) {
  for (uint8_t i = 0; i <= x; i++) {
    chip->memory[chip->index] = chip->registers[i];
    chip->index++;
  }
}

static void op_Fx65(Chip8* chip, uint16_t x) {
  for (uint8_t i = 0; i <= x; i++) {
    chip->registers[i] = chip->memory[chip->index];
    chip->index++;
  }
}

// 00E0: Clear display
static void op_00E0(Chip8* chip) {
  memset(chip->pixels, 0, sizeof(chip->pixels));
}

// 00EE: Return from a subroutine
// read decrement
static void op_00EE(Chip8* chip) {
  chip->program_counter = chip->stack[--chip->stack_pointer];
}

// 1nnn: Jump to address nnn
static void op_1nnn(Chip8* chip, uint16_t nnn) {
  chip->program_counter = nnn;
}

// 2nnn: Call subroutine at address
static void op_2nnn(Chip8* chip, uint16_t nnn) {
  chip->stack[chip->stack_pointer++] = chip->program_counter;

  chip->program_counter = nnn;
}

// 3xkk: Skip next instruction if Fx == kk
static void op_3xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  if (chip->registers[x] == kk) {
    chip->program_counter += 2;
  }
}

static void op_4xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  if (chip->registers[x] != kk) {
    chip->program_counter += 2;
  }
}

static void op_5xy0(Chip8* chip, uint16_t x, uint16_t y) {
  if (chip->registers[x] == chip->registers[y]) {
    chip->program_counter += 2;
  }
}

static void op_6xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  chip->registers[x] = kk;
}

static void op_7xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  chip->registers[x] += kk;
}


static void op_8xy0(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] = chip->registers[y];
}

static void op_8xy1(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] |= chip->registers[y];
  chip->registers[0xF] = 0;
}

static void op_8xy2(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] &= chip->registers[y];
  chip->registers[0xF] = 0;
}

static void op_8xy3(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] ^= chip->registers[y];
  chip->registers[0xF] = 0;
}

static void op_8xy4(Chip8* chip, uint16_t x, uint16_t y) {
  uint16_t sum = chip->registers[x] + chip->registers[y];
  chip->registers[x] = sum & 0xFF;
  chip->registers[0xF] = (sum > 0xFF);

}

static void op_8xy5(Chip8* chip, uint16_t x, uint16_t y) {
  uint16_t difference = chip->registers[x] - chip->registers[y];
  chip->registers[x] = difference & 0xFF;
  chip->registers[0xF] = (difference < 0xFF);
}

static void op_8xy6(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] = chip->registers[y];
  uint16_t temp = (chip->registers[x] & 0x1);
  chip->registers[x] >>= 1;
  chip->registers[0xF] = temp;
}

static void op_8xy7(Chip8* chip, uint16_t x, uint16_t y) {
  uint16_t difference = chip->registers[y] - chip->registers[x];
  chip->registers[x] = difference & 0xFF;
  chip->registers[0xF] = (difference < 0xFF);
}

static void op_8xyE(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] = chip->registers[y];
  uint16_t temp = (chip->registers[x] & 0x80) >> 7;
  chip->registers[x] <<= 1;
  chip->registers[0xF] = temp;
}

static void op_9xy0(Chip8* chip, uint16_t x, uint16_t y) {
  if (chip->registers[x] != chip->registers[y]) {
    chip->program_counter += 2;
  }
}

static void op_Annn(Chip8* chip, uint16_t nnn) {
  chip->index = nnn;
}

static void op_Bnnn(Chip8* chip, uint16_t nnn) {
  chip->program_counter = nnn + chip->registers[0];
}

static void op_Cxkk(Chip8* chip, uint16_t x, uint16_t kk) {
  chip->registers[x] = rand() & kk;
}

static void op_Dxyn(Chip8* chip, uint16_t x, uint16_t y, uint16_t n) {
  chip->registers[0xF] = 0;

  uint8_t x_pos = chip->registers[x] % 64;
  uint8_t y_pos = chip->registers[y] % 32;

  for (uint8_t row = 0; row < n; ++row) {
    if (y_pos + row >= 32) break;
    uint8_t sprite_byte = chip->memory[chip->index + row];

    for (uint8_t col = 0; col < 8; ++col) {
      if (x_pos + col >= 64) break;

      uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
      uint32_t* screen_pixel = &chip->pixels[(y_pos + row) * 64 + (x_pos + col)];

      if (sprite_pixel) {
        if (*screen_pixel == 0xFFFFFFFF) {
          chip->registers[0xF] = 1;
        }

        *screen_pixel ^= 0xFFFFFFFF;
      }
    }
  }
}

// https://multigesture.net/wp-content/uploads/mirror/zenogais/FunctionPointers.htm
static void op_NULL() {
  printf("Unknown opcode: %X", opcode);
  exit(EXIT_FAILURE);
};

void cycle(Chip8* chip) {
  opcode = (chip->memory[chip->program_counter] << 8u) |
    chip->memory[chip->program_counter + 1];
  op = opcode & 0xF000;
  x = (opcode & 0x0F00) >> 8;
  y = (opcode & 0x00F0) >> 4;
  n = opcode & 0x000F;
  kk = opcode & 0x00FF;
  nnn = opcode & 0x0FFF;

  chip->program_counter += 2;

  switch (op) {
    case 0x8000:
      table8[n](chip, x, y);
      break;
    case 0x0000:
      table0[n](chip);
      break;
    case 0xE000:
      tableE[n](chip, x);
      break;
    case 0xF000:
      tableF[kk](chip, x);
      break;
    default:
      switch (op) {
        case 0x1000:
          op_1nnn(chip, nnn);
          break;
        case 0x2000:
          op_2nnn(chip, nnn);
          break;
        case 0x3000:
          op_3xkk(chip, x, kk);
          break;
        case 0x4000:
          op_4xkk(chip, x, kk);
          break;
        case 0x5000:
          op_5xy0(chip, x, y);
          break;
        case 0x6000:
          op_6xkk(chip, x, kk);
          break;
        case 0x7000:
          op_7xkk(chip, x, kk);
          break;
        case 0x9000:
          op_9xy0(chip, x, y);
          break;
        case 0xA000:
          op_Annn(chip, nnn);
          break;
        case 0xB000:
          op_Bnnn(chip, nnn);
          break;
        case 0xC000:
          op_Cxkk(chip, x, kk);
          break;
        case 0xD000:
          op_Dxyn(chip, x, y, n);
          break;
        default:
          printf("Unknown opcode: %XX", opcode);
          exit(EXIT_FAILURE);
          break;
      }
      break;
  }
}

void update_timers(Chip8* chip) {
  if (chip->delay_timer > 0)
  {
    --chip->delay_timer;
  }

  if (chip->sound_timer > 0)
  {
    --chip->sound_timer;
  }
}
