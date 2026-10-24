#include "chip.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

void op_00E0() {}

void op_00EE() {}

void op_1nnn(Chip8* chip, uint16_t nnn) {
  chip->program_counter = nnn;
}

void op_2nnn(Chip8* chip, uint16_t nnn) {
  chip->stack_pointer += 1;
  chip->stack[chip->stack_pointer] = chip->program_counter;
  chip->program_counter = nnn;
}

void op_3xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  if (chip->registers[x] == kk) {
    chip->program_counter += 2;
  }
}

void op_4xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  if (chip->registers[x] != kk) {
    chip->program_counter += 2;
  }
}

void op_5xy0(Chip8* chip, uint16_t x, uint16_t y) {
  if (chip->registers[x] == chip->registers[y]) {
    chip->program_counter += 2;
  }
}

void op_6xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  chip->registers[x] = kk;
}

void op_7xkk(Chip8* chip, uint16_t x, uint16_t kk) {
  chip->registers[x] += kk;
}

void op_9xy0(Chip8* chip, uint16_t x, uint16_t y) {
  if (chip->registers[x] != chip->registers[y]) {
    chip->program_counter += 2;
  }
}

void op_Annn(Chip8* chip, uint16_t nnn) {
  chip->index = nnn;
}

void op_Bnnn(Chip8* chip, uint16_t nnn) {
  chip->program_counter += nnn + chip->registers[0];
}

void op_Cxkk(Chip8* chip, uint16_t x, uint16_t kk) {
  // Not sure if this is adequate
  chip->registers[x] = rand() & kk;
}

void op_Dxyn(Chip8* chip, uint16_t x, uint16_t y, uint16_t n) {
  chip->registers[0xF] = 0;

  uint8_t x_pos = chip->registers[x] % 64;
  uint8_t y_pos = chip->registers[y] % 32;

  for (unsigned int row = 0; row < n; ++row) {
    uint8_t sprite_byte = chip->memory[chip->index + row];

    for (unsigned int col = 0; col < 8; ++col) {
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

void cycle(Chip8* chip) {
  chip->opcode = (chip->memory[chip->program_counter] << 8u) |
    chip->memory[chip->program_counter + 1];

  // Is there a real reason the chip has to know the opcode?
  uint16_t op = chip->opcode & 0xF000;
  uint16_t x = chip->opcode & 0x0F00;
  uint16_t y = chip->opcode & 0x00F0;
  uint16_t n = chip->opcode & 0x000F;
  uint16_t kk = chip->opcode & 0x00FF;
  uint16_t nnn = chip->opcode & 0x0FFF;

  chip->program_counter += 2;

  switch (op) {
    case 0x8000:
      break;
    case 0x0000:
      break;
    case 0xE000:
      break;
    case 0xF000:
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
          perror("Unknown opcode");
          exit(EXIT_FAILURE);
          break;
      }
      break;
  }

}

