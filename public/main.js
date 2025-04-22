import ModuleFactory from "/build/main.js";

const init = async () => {
  const Module = await ModuleFactory();

  const isElf = Module.cwrap('is_elf', 'number', ['number', 'number']);
  const getEntry = Module.cwrap('get_entry_point', 'number', ['number', 'number']);
  const getSections = Module.cwrap('list_sections', 'string', ['number', 'number']);

  const uploadBtn = document.getElementById("uploadBtn");
  const output = document.getElementById("output");

  uploadBtn.onclick = async () => {
    const file = fileInput.files[0];

    const status = document.getElementById("status");
    const entryPoint = document.getElementById("entryPoint");
    const sectionsTitle = document.getElementById("sectionsTitle");
    const sectionsList = document.getElementById("sectionsList");

    status.textContent = '';
    entryPoint.textContent = '';
    sectionsTitle.style.display = 'none';
    sectionsList.innerHTML = '';
    if (!file) {
      output.textContent = "Please select a file.";
      return;
    }

    const buffer = await file.arrayBuffer();
    const len = buffer.byteLength;
    const ptr = Module._malloc(len);
    Module.HEAPU8.set(new Uint8Array(buffer), ptr);

    const result = isElf(ptr, len);
    status.textContent = result ? "✅ Valid ELF file." : "❌ Not an ELF file.";

    if (result) {
      const entry = getEntry(ptr, len);
      entryPoint.textContent = `Entry point: 0x${entry.toString(16)}`;

      const section_raw = getSections(ptr, len);
      try {
        const sections = JSON.parse(section_raw);
        if (Array.isArray(sections)) {
          sectionsTitle.style.display = 'block';
          for (const sec of sections) {
            const li = document.createElement('li');
            li.textContent = sec;
            sectionsList.appendChild(li);
          }
        }
      } catch (e) {
        const li = document.createElement('li');
        li.textContent = 'Error parsing section list.';
        sectionsList.appendChild(li);
      }
    }

    Module._free(ptr);
  };
};

document.addEventListener("DOMContentLoaded", init);
