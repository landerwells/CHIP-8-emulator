
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


  // Allocate a buffer to hold 16 bytes of binary data
  unsigned char buffer[16];
  size_t bytesRead = fread(buffer, sizeof(unsigned char), 16, file);

  // Print the bytes in hexadecimal format
  printf("Read %zu bytes:\n", bytesRead);
  for (size_t i = 0; i < bytesRead; i++) {
    printf("%02X ", buffer[i]);
  }
  printf("\n");

  fclose(file); // Close the file

}
