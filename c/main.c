#include "struct.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *json_buff = NULL;

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

EMSCRIPTEN_KEEPALIVE
const char *list_sections(const uint8_t *bytes, int length) {
  if (!is_elf(bytes, length)) {
    printf("Not a valid ELF file.\n");
    return 0;
  }

  free(json_buff);
  json_buff = NULL;
  size_t cap = 4096;
  json_buff = malloc(cap);
  strcpy(json_buff, "[");

  if (bytes[EI_CLASS] == ELFCLASS32) {
    Elf32 *ehdr = (Elf32 *)bytes;
    Elf32_Shdr *sh_table = (Elf32_Shdr *)(bytes + ehdr->e_shoff);
    const char *shstrtab =
        (const char *)(bytes + sh_table[ehdr->e_shstrndx].sh_offset);

    for (int i = 0; i < ehdr->e_shnum; i++) {
      const char *name = shstrtab + sh_table[i].sh_name;
      strcat(json_buff, "\"");
      strcat(json_buff, name);
      strcat(json_buff, "\"");
      if (i != ehdr->e_shnum - 1)
        strcat(json_buff, ",");
    }
  } else if (bytes[EI_CLASS] == ELFCLASS64) {
    Elf64 *ehdr = (Elf64 *)bytes;
    Elf64_Shdr *sh_table = (Elf64_Shdr *)(bytes + ehdr->e_shoff);
    const char *shstrtab =
        (const char *)(bytes + sh_table[ehdr->e_shstrndx].sh_offset);

    for (int i = 0; i < ehdr->e_shnum; i++) {
      const char *name = shstrtab + sh_table[i].sh_name;
      strcat(json_buff, "\"");
      strcat(json_buff, name);
      strcat(json_buff, "\"");
      if (i != ehdr->e_shnum - 1)
        strcat(json_buff, ",");
    }
  }

  strcat(json_buff, "]");
  return json_buff;
}
