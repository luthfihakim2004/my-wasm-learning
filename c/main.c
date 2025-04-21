#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define ELFCLASS32 1
#define ELFCLASS64 2
#define EI_NIDENT 16
#define EI_CLASS 4

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint32_t e_entry;
  uint32_t e_phoff;
  uint32_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} Elf32;

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint32_t e_phoff;
  uint32_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} Elf64;

char *saved_input = NULL;

EMSCRIPTEN_KEEPALIVE
int is_elf(const uint8_t *bytes, int length) {
  if (length < 4)
    return 0;
  return bytes[0] == 0x7f && bytes[1] == 'E' && bytes[2] == 'L' &&
         bytes[3] == 'F';
}

EMSCRIPTEN_KEEPALIVE
uint64_t get_entry_point(const uint8_t *bytes, int length) {
  if (!is_elf(bytes, length))
    return 0;

  if (bytes[EI_CLASS] == ELFCLASS32) {
    if (length < sizeof(Elf32))
      return 0;
    Elf32 *hdr = (Elf32 *)bytes;
    return hdr->e_entry;
  } else if (bytes[EI_CLASS] == ELFCLASS64) {
    if (length < sizeof(Elf64))
      return 0;
    Elf64 *hdr = (Elf64 *)bytes;
    return hdr->e_entry;
  }

  return 0;
}
