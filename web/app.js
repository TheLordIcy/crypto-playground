// Vanilla JS. Every demo displays intermediate values, not just results.
const demo = document.getElementById('demo');

document.querySelectorAll('nav button').forEach((btn) => {
  btn.addEventListener('click', () => {
    demo.textContent = btn.dataset.demo + ' demo: not implemented yet';
  });
});
