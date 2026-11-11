// Vanilla JS. No framework. WASM entry points are wired in Phase 6.
const tabs = document.querySelectorAll('.tab');
const traceEl = document.getElementById('trace');
const statusEl = document.getElementById('status');
const runBtn = document.getElementById('run');
const stepBtn = document.getElementById('step');
const resetBtn = document.getElementById('reset');
const presetSel = document.getElementById('preset');

let steps = [];
let cursor = 0;
let running = false;

function setStatus(text, kind) {
  statusEl.textContent = text;
  statusEl.className = 'status' + (kind ? ' ' + kind : '');
}

function renderTrace() {
  traceEl.innerHTML = '';
  steps.slice(0, cursor).forEach((s, i) => {
    const li = document.createElement('li');
    li.innerHTML = `<span class="idx">#${i + 1}</span><span class="val">${escapeHtml(s)}</span>`;
    traceEl.appendChild(li);
  });
  traceEl.scrollTop = traceEl.scrollHeight;
}

function escapeHtml(t) {
  return String(t).replace(/[&<>"']/g, (c) => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
}

// Demo step generators (stubs until WASM lands). Each returns an array of
// human-readable intermediate-value lines.
const demos = {
  rsa: () => [
    'Preset selected; generating primes p, q...',
    'p = (stub) 0x...',
    'q = (stub) 0x...',
    'n = p*q = (stub)',
    'phi(n) = (p-1)(q-1) = (stub)',
    'e = 65537',
    'd = e^-1 mod phi(n) = (stub)',
    'm (input) -> c = m^e mod n = (stub)',
    'decrypt: m = c^d mod n = (stub)  [round-trip OK]',
  ],
  dh: () => [
    'Agreed params: p (RFC 7919 group), g',
    'Alice private a, public A = g^a mod p',
    'Bob private b, public B = g^b mod p',
    'Alice shared = B^a mod p',
    'Bob shared = A^b mod p',
    'shared secrets match: (stub)',
  ],
  hash: () => [
    'Input padded to multiple of 512 bits',
    'Block 0: message schedule W[0..63]',
    'Compression: a..h updated over 64 rounds',
    'Final digest (SHA-256) = (stub)',
  ],
  primality: () => [
    'Candidate n = (stub)',
    'Miller-Rabin round 1: witness a=2 -> (stub)',
    'Miller-Rabin round 2: witness a=3 -> (stub)',
    'Result: probably prime',
  ],
};

let currentDemo = 'rsa';

tabs.forEach((t) => {
  t.addEventListener('click', () => {
    tabs.forEach((x) => x.classList.remove('active'));
    t.classList.add('active');
    currentDemo = t.dataset.demo;
    reset();
  });
});

function reset() {
  steps = demos[currentDemo]();
  cursor = 0;
  renderTrace();
  setStatus('idle');
  running = false;
}

function step() {
  if (cursor < steps.length) {
    cursor++;
    renderTrace();
    setStatus(cursor === steps.length ? 'done' : 'running', cursor === steps.length ? 'done' : 'running');
  }
}

function runAll() {
  if (running) return;
  running = true;
  setStatus('running', 'running');
  const tick = () => {
    if (cursor < steps.length) {
      cursor++;
      renderTrace();
      setTimeout(tick, 250);
    } else {
      setStatus('done', 'done');
      running = false;
    }
  };
  tick();
}

runBtn.addEventListener('click', runAll);
stepBtn.addEventListener('click', step);
resetBtn.addEventListener('click', reset);
presetSel.addEventListener('change', () => setStatus('preset: ' + presetSel.value));

reset();
setStatus('idle');
