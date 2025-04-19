import createModule from '/build/hello.js';

const output = document.getElementById('output');
const greetBtn = document.getElementById('greetBtn');

createModule().then((Module) => {
  const greet = Module.cwrap('greet', 'number', ['string']);

  greetBtn.addEventListener('click', () => {
    const name = document.getElementById('name').value;
    greet(name); // prints to console
    output.textContent = `Hello, ${name}! (from WebAssembly)`;
  });
});
