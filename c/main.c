#include "struct.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// for checkin magic number ".ELF" of file
EMSCRIPTEN_KEEPALIVE
int is_elf(const uint8_t *bytes, int length) {
  if (length < 4)
    return 0;
  return bytes[0] == 0x7f && bytes[1] == 'E' && bytes[2] == 'L' &&
         bytes[3] == 'F';
}

// lookin for entry point of the file
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

// get all available section of the ELF file
EMSCRIPTEN_KEEPALIVE
const char *list_sections(const uint8_t *bytes, int length) {
  if (!is_elf(bytes, length)) {
    printf("Not a valid ELF file.\n");
    return NULL;
  }

  char *json_buff = NULL;

  if (json_buff)
    free(json_buff);
  size_t cap = 4096;
  json_buff = malloc(cap);
  if (!json_buff)
    return NULL;
  json_buff[0] = '\0';
  strcpy(json_buff, "[");

  int is_64 = bytes[EI_CLASS] == ELFCLASS64;

  void *ehdr = (void *)bytes;
  const void *sh_table =
      bytes + (is_64 ? ((Elf64 *)ehdr)->e_shoff : ((Elf32 *)ehdr)->e_shoff);
  int shnum = is_64 ? ((Elf64 *)ehdr)->e_shnum : ((Elf32 *)ehdr)->e_shnum;
  int shstrndx =
      is_64 ? ((Elf64 *)ehdr)->e_shstrndx : ((Elf32 *)ehdr)->e_shstrndx;

  size_t sh_entry_size = is_64 ? sizeof(Elf64_Shdr) : sizeof(Elf32_Shdr);
  const char *shstrtab =
      (const char *)(bytes +
                     (is_64 ? ((Elf64_Shdr *)(sh_table))[shstrndx].sh_offset
                            : ((Elf32_Shdr *)(sh_table))[shstrndx].sh_offset));

  for (int i = 0; i < shnum; i++) {
    const char *name = shstrtab + (is_64 ? ((Elf64_Shdr *)sh_table)[i].sh_name
                                         : ((Elf32_Shdr *)sh_table)[i].sh_name);

    if (strlen(json_buff) + strlen(name) + 4 >= cap) {
      cap *= 2;
      char *tmp = realloc(json_buff, cap);
      if (!tmp)
        return NULL;
      json_buff = tmp;
    }

    strncat(json_buff, "\"", cap);
    strncat(json_buff, name, cap);
    strncat(json_buff, "\"", cap);
    if (i != shnum - 1)
      strncat(json_buff, ",", cap);
  }

  strncat(json_buff, "]", cap);
  return json_buff;
}
