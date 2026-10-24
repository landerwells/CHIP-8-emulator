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

  fclose(file); // Close the file

}


void cycle(Chip8* chip) {
  chip->opcode = (chip->memory[chip->program_counter] << 8u) |
    chip->memory[chip->program_counter + 1];

  printf("opcode: %d\n", chip->opcode);

  chip->program_counter += 2;
}

