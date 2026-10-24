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

// The next two are both keyboard instructions.
void op_ExA1(Chip8* chip, uint16_t x) {
}

void op_Ex9E(Chip8* chip, uint16_t x) {
}

void op_Fx07(Chip8* chip, uint16_t x) {
  chip->registers[x] = chip->delay_timer;

}

// Another keyboard instruction.
void op_Fx0A(Chip8* chip, uint16_t x) {

}

void op_Fx15(Chip8* chip, uint16_t x) {
  chip->delay_timer = chip->registers[x];
}

void op_Fx18(Chip8* chip, uint16_t x) {
  chip->sound_timer = chip->registers[x];
}

void op_Fx1E(Chip8* chip, uint16_t x) {
  chip->index += chip->registers[x];
}

void op_Fx29(Chip8* chip, uint16_t x) {
  // Not sure what needs to be done here, look at implementation

}

void op_Fx33(Chip8* chip, uint16_t x) {

}
void op_Fx55(Chip8* chip, uint16_t x) {}
void op_Fx65(Chip8* chip, uint16_t x) {}

void op_00E0(Chip8* chip) {
  memset(chip->pixels, 0, sizeof(chip->pixels));
}

// Return: Need to rectify this and op_2nnn()
void op_00EE(Chip8* chip) {
  chip->stack_pointer--;
  chip->program_counter = chip->stack[chip->stack_pointer];
}

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


void op_8xy0(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] = chip->registers[y];
}

void op_8xy1(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] |= chip->registers[y];
}

void op_8xy2(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] &= chip->registers[y];
}

void op_8xy3(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[x] ^= chip->registers[y];
}

void op_8xy4(Chip8* chip, uint16_t x, uint16_t y) {
  uint16_t sum = chip->registers[x] + chip->registers[y];
  chip->registers[0xF] = (sum > 0xFF);
  chip->registers[x] = sum & 0xFF;
}

void op_8xy5(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[0xF] = (chip->registers[x] > chip->registers[y]);
  chip->registers[x] -= chip->registers[y];
}

void op_8xy6(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[0xF] = (chip->registers[x] & 0x1);
  chip->registers[x] >>= 1;
}

void op_8xy7(Chip8* chip, uint16_t x, uint16_t y) {

  chip->registers[0xF] = (chip->registers[y] > chip->registers[x]);
  chip->registers[x] = chip->registers[y] - chip->registers[x];
}

void op_8xyE(Chip8* chip, uint16_t x, uint16_t y) {
  chip->registers[0xF] = (chip->registers[x] & 0x80) >> 7;
  chip->registers[x] <<= 1;
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
          perror("Unknown opcode");
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
          perror("Unknown opcode");
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
          perror("Unknown opcode");
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
          perror("Unknown opcode");
          exit(EXIT_FAILURE);
          break;
      }
      break;
  }

}

