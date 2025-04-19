import ModuleFactory from "/build/hello.js";

const init = async () => {
  const Module = await ModuleFactory();

  const store_input = Module.cwrap("store_input", null, ["number"]);
  const get_input = Module.cwrap("get_input", "number", []);

  const nameInput = document.getElementById("name");
  const greetBtn = document.getElementById("greetBtn");
  const output = document.getElementById("output");

  greetBtn.onclick = () => {
    const inputText = nameInput.value;

    // Allocate memory
    const len = Module.lengthBytesUTF8(inputText) + 1;
    const ptr = Module._malloc(len);

    // Copy string into WASM memory
    Module.stringToUTF8(inputText, ptr, len);

    // Call WASM function
    store_input(ptr);

    // Free memory
    Module._free(ptr);

    // Get result from WASM
    const resultPtr = get_input();
    const resultStr = Module.UTF8ToString(resultPtr);

    output.textContent = `Hello, ${resultStr}!`;
  };
};

document.addEventListener("DOMContentLoaded", init);
