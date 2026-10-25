#include "chip.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

  fclose(file); // Close the file

}

void init_chip(Chip8* chip) {
  chip->program_counter = START_LOC;

  const unsigned int FONTSET_SIZE = 80;

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

  for (unsigned int i = 0; i < FONTSET_SIZE; i++) {
    chip->memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }

}


// The next two are both keyboard instructions.
void op_ExA1(Chip8* chip, uint16_t x) {
  printf("op_ExA1\n");

  if (!chip->keypad[chip->registers[x]])
  {
    chip->program_counter += 2;
  }

}

void op_Ex9E(Chip8* chip, uint16_t x) {
  printf("op_Ex9E\n");

  if (chip->keypad[chip->registers[x]])
  {
    chip->program_counter += 2;
  }
}

void op_Fx07(Chip8* chip, uint16_t x) {
  printf("op_Fx07");
  chip->registers[x] = chip->delay_timer;

}

// Another keyboard instruction.
void op_Fx0A(Chip8* chip, uint16_t x) {
  printf("op_Fx0A Still needs to be implemented\n");

  if (chip->keypad[0])
  {
    chip->registers[x] = 0;
  }
  else if (chip->keypad[1])
  {
    chip->registers[x] = 1;
  }
  else if (chip->keypad[2])
  {
    chip->registers[x] = 2;
  }
  else if (chip->keypad[3])
  {
    chip->registers[x] = 3;
  }
  else if (chip->keypad[4])
  {
    chip->registers[x] = 4;
  }
  else if (chip->keypad[5])
  {
    chip->registers[x] = 5;
  }
  else if (chip->keypad[6])
  {
    chip->registers[x] = 6;
  }
  else if (chip->keypad[7])
  {
    chip->registers[x] = 7;
  }
  else if (chip->keypad[8])
  {
    chip->registers[x] = 8;
  }
  else if (chip->keypad[9])
  {
    chip->registers[x] = 9;
  }
  else if (chip->keypad[10])
  {
    chip->registers[x] = 10;
  }
  else if (chip->keypad[11])
  {
    chip->registers[x] = 11;
  }
  else if (chip->keypad[12])
  {
    chip->registers[x] = 12;
  }
  else if (chip->keypad[13])
  {
    chip->registers[x] = 13;
  }
  else if (chip->keypad[14])
  {
    chip->registers[x] = 14;
  }
  else if (chip->keypad[15])
  {
    chip->registers[x] = 15;
  }
  else
  {
    chip->program_counter -= 2;
  }
}

void op_Fx15(Chip8* chip, uint16_t x) {
  printf("op_Fx15\n");
  chip->delay_timer = chip->registers[x];
}

void op_Fx18(Chip8* chip, uint16_t x) {
  printf("op_Fx18\n");
  chip->sound_timer = chip->registers[x];
}

void op_Fx1E(Chip8* chip, uint16_t x) {
  printf("op_Fx1E\n");
  chip->index += chip->registers[x];
}

void op_Fx29(Chip8* chip, uint16_t x) {
  printf("op_Fx29\n");
  chip->index = FONTSET_START_ADDRESS + (5 * chip->registers[x]);

}

void op_Fx33(Chip8* chip, uint16_t x) {
  printf("op_Fx33\n");
  uint8_t value = chip->registers[x];

  chip->memory[chip->index + 2] = value % 10;
  value /= 10;

  chip->memory[chip->index + 1] = value % 10;
  value /= 10;

  chip->memory[chip->index] = value % 10;
}

// Fx55: Stores registers V0 through Vx in memory starting at location I
void op_Fx55(Chip8* chip, uint16_t x) {
  printf("op_Fx55\n");
  for (uint8_t i = 0; i <= x; i++) {
    chip->memory[chip->index + i] = chip->registers[i];
  }
}

void op_Fx65(Chip8* chip, uint16_t x) {
  printf("op_Fx65\n");
  for (uint8_t i = 0; i <= chip->registers[x]; i++) {
    chip->registers[i] = chip->memory[chip->index + i];
  }
}

// 00E0: Clear display
void op_00E0(Chip8* chip) {
  printf("op_00E0\n");
  memset(chip->pixels, 0, sizeof(chip->pixels));
}

// 00EE: Return from a subroutine
void op_00EE(Chip8* chip) {
  printf("op_00EE\n");
  chip->program_counter = chip->stack[chip->stack_pointer];
  chip->stack_pointer--;
}

// 1nnn: Jump to address nnn
void op_1nnn(Chip8* chip, uint16_t nnn) {
  printf("op_1nnn\n");
  chip->program_counter = nnn;
}

// 2nnn: Call subroutine at address
void op_2nnn(Chip8* chip, uint16_t nnn) {
  printf("op_2nnn\n");
  // Seems after swapping these two lines we end up in an infinite loop?
  chip->stack_pointer += 1;
  chip->stack[chip->stack_pointer] = chip->program_counter;

  chip->program_counter = nnn;
}

// 3xkk: Skip next instruction if Fx == kk
void op_3xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  printf("op_3xkk\n");
  if (chip->registers[x] == kk) {
    chip->program_counter += 2;
  }
}

void op_4xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  printf("op_4xkk\n");
  if (chip->registers[x] != kk) {
    chip->program_counter += 2;
  }
}

void op_5xy0(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_5xy0\n");
  if (chip->registers[x] == chip->registers[y]) {
    chip->program_counter += 2;
  }
}

void op_6xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  printf("op_6xkk\n");
  chip->registers[x] = kk;
}

void op_7xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  printf("op_7xkk\n");
  chip->registers[x] += kk;
}


void op_8xy0(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy0\n");
  chip->registers[x] = chip->registers[y];
}

void op_8xy1(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy1\n");
  chip->registers[x] |= chip->registers[y];
}

void op_8xy2(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy2\n");
  chip->registers[x] &= chip->registers[y];
}

void op_8xy3(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy3\n");
  chip->registers[x] ^= chip->registers[y];
}

void op_8xy4(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy4\n");
  uint16_t sum = chip->registers[x] + chip->registers[y];
  chip->registers[0xF] = (sum > 0xFF);
  chip->registers[x] = sum & 0xFF;
}

void op_8xy5(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy5\n");
  chip->registers[0xF] = (chip->registers[x] > chip->registers[y]);
  chip->registers[x] -= chip->registers[y];
}

void op_8xy6(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy6\n");
  chip->registers[0xF] = (chip->registers[x] & 0x1);
  chip->registers[x] >>= 1;
}

void op_8xy7(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xy7\n");
  chip->registers[0xF] = (chip->registers[y] > chip->registers[x]);
  chip->registers[x] = chip->registers[y] - chip->registers[x];
}

void op_8xyE(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_8xyE\n");
  chip->registers[0xF] = (chip->registers[x] & 0x80) >> 7;
  chip->registers[x] <<= 1;
}

void op_9xy0(Chip8* chip, uint16_t x, uint16_t y) {
  printf("op_9xy0\n");
  if (chip->registers[x] != chip->registers[y]) {
    chip->program_counter += 2;
  }
}

void op_Annn(Chip8* chip, uint16_t nnn) {
  printf("op_Annn\n");
  chip->index = nnn;
}

void op_Bnnn(Chip8* chip, uint16_t nnn) {
  printf("op_Bnnn\n");
  chip->program_counter += nnn + chip->registers[0];
}

void op_Cxkk(Chip8* chip, uint16_t x, uint16_t kk) {
  printf("op_Cxkk\n");
  chip->registers[x] = rand() & kk;
}

void op_Dxyn(Chip8* chip, uint16_t x, uint16_t y, uint16_t n) {
  printf("op_Dxyn\n");
  chip->registers[0xF] = 0;

  uint8_t x_pos = chip->registers[x] % 64;
  uint8_t y_pos = chip->registers[y] % 32;

  for (unsigned int row = 0; row < n; ++row) {
    if (y_pos + row >= 32) return;
    uint8_t sprite_byte = chip->memory[chip->index + row];

    for (unsigned int col = 0; col < 8; ++col) {
      if (x_pos + col >= 64) return;

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

// I think I can organize this a bit better, but otherwise I like the
// way it splits things off.
void cycle(Chip8* chip) {
  uint16_t opcode = (chip->memory[chip->program_counter] << 8u) |
    chip->memory[chip->program_counter + 1];
  uint16_t op = opcode & 0xF000;
  uint16_t x = (opcode & 0x0F00) >> 8;
  uint16_t y = (opcode & 0x00F0) >> 4;
  uint16_t n = opcode & 0x000F;
  uint16_t kk = opcode & 0x00FF;
  uint16_t nnn = opcode & 0x0FFF;

  chip->program_counter += 2;

  switch (op) {
    case 0x8000:
      switch (n) {
        case 0x0:
          op_8xy0(chip, x, y);
          break;
        case 0x1:
          op_8xy1(chip, x, y);
          break;
        case 0x2:
          op_8xy2(chip, x, y);
          break;
        case 0x3:
          op_8xy3(chip, x, y);
          break;
        case 0x4:
          op_8xy4(chip, x, y);
          break;
        case 0x5:
          op_8xy5(chip, x, y);
          break;
        case 0x6:
          op_8xy6(chip, x, y);
          break;
        case 0x7:
          op_8xy7(chip, x, y);
          break;
        case 0xE:
          op_8xyE(chip, x, y);
          break;
        default:
          printf("Unknown opcode: %X", chip->opcode);
          exit(EXIT_FAILURE);
          break;
      }
      break;
    case 0x0000:
      switch (n) {
        case 0x0:
          op_00E0(chip);
          break;
        case 0xE:
          op_00EE(chip);
          break;
      }
      break;
    case 0xE000:
      switch (kk) {
        case 0xA1:
          op_ExA1(chip, x);
          break;
        case 0x9E:
          op_Ex9E(chip, x);
          break;
        default:
          printf("Unknown opcode: %X", chip->opcode);
          exit(EXIT_FAILURE);
          break;
      }
      break;
    case 0xF000:
      switch (kk) {
        case 0xA1:
          op_Fx07(chip, x);
          break;
        case 0x0A:
          op_Fx0A(chip, x);
          break;
        case 0x15:
          op_Fx15(chip, x);
          break;
        case 0x18:
          op_Fx18(chip, x);
          break;
        case 0x1E:
          op_Fx1E(chip, x);
          break;
        case 0x29:
          op_Fx29(chip, x);
          break;
        case 0x33:
          op_Fx33(chip, x);
          break;
        case 0x55:
          op_Fx55(chip, x);
          break;
        case 0x65:
          op_Fx65(chip, x);
          break;
        default:
          printf("Unknown opcode: %X", chip->opcode);
          exit(EXIT_FAILURE);
          break;
      }
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
          printf("Unknown opcode: %X", chip->opcode);
          exit(EXIT_FAILURE);
          break;
      }
      break;
  }

  // Decrement the delay timer if it's been set
  if (chip->delay_timer > 0)
  {
    --chip->delay_timer;
  }

  // Decrement the sound timer if it's been set
  if (chip->sound_timer > 0)
  {
    --chip->sound_timer;
  }

}

