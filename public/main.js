import ModuleFactory from "/build/hello.js";

const init = async () => {
  const Module = await ModuleFactory();

  const isElf = Module.cwrap('is_elf', 'number', ['number', 'number']);
  const getEntry = Module.cwrap('get_entry_point', 'number', ['number', 'number']);

  const uploadBtn = document.getElementById("uploadBtn");
  const output = document.getElementById("output");

  uploadBtn.onclick = async () => {
    const file = fileInput.files[0];
    if (!file) return;

    const buffer = await file.arrayBuffer();
    const len = buffer.byteLength;
    const ptr = Module._malloc(len);
    Module.HEAPU8.set(new Uint8Array(buffer), ptr);

    const result = isElf(ptr, len);
    output.textContent = result ? "Valid ELF file." : "Not an ELF file.";

    if (result) {
      const entry = getEntry(ptr, len);
      output.textContent += `\nEntry point: 0x${entry.toString(16)}`;
    }

    Module._free(ptr);
  };
};

document.addEventListener("DOMContentLoaded", init);
