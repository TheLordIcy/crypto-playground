# Crypto Playground — Roadmap

An interactive, step-by-step cryptography workbench. All arithmetic is implemented
from scratch in C and compiled to WebAssembly; the interface is plain HTML/CSS/JS
and shows every intermediate value instead of only the final answer.

**Purpose:** demonstrate hard number-theoretic mathematics and low-level C
engineering in one project.

**Not a security tool.** This code is educational. It is not constant-time and
must never be used to protect real data.

---

## Core constraint

**No bignum library.** No GMP, no OpenSSL, no libsodium, no `__int128` shortcuts
for the big-integer layer. Writing the arbitrary-precision arithmetic is the
project. The moment a library is linked, this becomes a UI exercise and the C
engineering disappears.

---

## Stack

| Layer | Choice | Why |
|---|---|---|
| Math / logic | C99, no dependencies | The point of the project |
| Bridge | Emscripten → WebAssembly | One `<script>` tag, ~20 lines of JS |
| UI | Plain HTML + CSS + vanilla JS | No npm, no bundler, no framework |
| Tests | Native C harness + official test vectors | Verifiable before any UI exists |

Build:

```
emcc core/*.c wasm/bindings.c -O2 -o web/crypto.js \
     -sEXPORTED_FUNCTIONS=... -sEXPORTED_RUNTIME_METHODS=...
python3 -m http.server 8000
```

---

## Architecture

```
core/      bignum.c  modmath.c  primes.c  rsa.c  dh.c  sha256.c  ecc.c
wasm/      bindings.c            # the only file that knows Emscripten exists
test/      run_tests.c  vectors/ # official test vectors
web/       index.html  style.css  app.js
```

`core/` is pure C99: no I/O, no globals, no allocation inside hot functions.
`wasm/bindings.c` is the sole bridge. `core/` also builds natively for testing
and benchmarking.

---

## Phases

### Phase 0 — Setup (2 days)

**Deliverables**
- `Makefile` with `make`, `make test`, `make bench`, `make asan`
- Flags: `-std=c99 -Wall -Wextra -Werror -pedantic`
- `test/run_tests.c` — tiny assert-based harness that reports pass/fail counts
- Fetch official vectors: RFC 8017 (RSA), FIPS 180-4 (SHA-256), RFC 7919 (DH groups)

**Done when:** `make test` runs and a deliberately broken assertion makes the
build fail.

---

### Phase 1 — Big integers (1 week)

The foundation everything else stands on.

**Deliverables**
- `bignum` struct: `uint32_t` limbs held in a `uint64_t` accumulator so carry
  arithmetic cannot overflow
- `add`, `sub`, `cmp`, `mul` (schoolbook), `divmod`, `shl`, `shr`
- Hex and decimal conversion both directions
- Sign handling (or a clearly documented magnitude-only design)

**Done when:** results match Python's arbitrary-precision integers on 10,000
randomized cases per operation, including division by values with multiple limbs.

**Risk:** long division is the hardest single algorithm in the project. Budget
more time than feels reasonable.

---

### Phase 2 — Modular arithmetic (4 days)

**Deliverables**
- `modpow` via square-and-multiply
- `modinv` via extended Euclid
- Reduction strategy: Barrett or Montgomery, chosen and justified in the README

**Done when:** `modpow` matches known vectors for exponents above 1024 bits, and
`modinv` round-trips (`a * a⁻¹ ≡ 1 mod m`).

---

### Phase 3 — Primality + RSA (1 week)

**Deliverables**
- Miller–Rabin, deterministic with fixed bases below 2⁶⁴
- Random prime generation of a given bit length
- RSA keygen, encrypt, decrypt
- Padding (PKCS#1 v1.5 or a documented simpler scheme)

**Done when:** messages round-trip, keypairs are generated from scratch per
session, and the implementation passes RFC 8017 test vectors.

---

### Phase 4 — Diffie–Hellman + SHA-256 (1 week)

**Deliverables**
- DH key exchange reusing `modpow`
- SHA-256: compression function, message schedule, padding

**Done when:** both simulated parties derive an identical shared secret, and
SHA-256 matches FIPS 180-4 vectors including the multi-block edge cases.

---

### Phase 5 — Elliptic curves (1–2 weeks, stretch)

**Deliverables**
- Field arithmetic modulo p
- Point addition, point doubling, scalar multiplication by double-and-add
- Curve: secp256k1 or P-256

**Done when:** point operations pass published test vectors.

**Cut this phase first if the schedule slips.** Phases 0–4 are already a
complete, defensible project.

---

### Phase 6 — WASM bridge + UI (1 week)

**Deliverables**
- `bindings.c` exposing four demos to JS
- One HTML page, one CSS file, vanilla JS
- Four demos: RSA, Diffie–Hellman, hashing, primality
- Every demo displays intermediate values — primes found, exponents,
  ciphertext blocks, rounds of Miller–Rabin — not just the result

**Done when:** a non-programmer can be walked through RSA in the browser and
follow what happens at each step.

---

### Phase 7 — Polish (3 days)

**Deliverables**
- Step traces that can be advanced one operation at a time
- Presets (e.g. 1024-bit vs 2048-bit keys)
- README with measured timings comparing key sizes
- Explicit "educational only, not constant-time" warning

---

## Testing strategy

1. **Test vectors first.** Every algorithm has published vectors; encode them
   before writing the implementation.
2. **Differential testing.** Compare bignum output against Python for the
   operations that are cheap to check.
3. **ASan + UBSan in debug.** Manual limb arithmetic produces out-of-bounds
   reads far more often than logic bugs. `-fsanitize=address,undefined` finds
   them immediately.
4. **Property checks.** `a * b / b == a`, `(a mod m) + (b mod m)` stays in range,
   SHA-256 of the empty string is the known constant.

---

## Rules

- C99 only. No C++, no extensions.
- Zero compiler warnings. Not "mostly clean."
- No allocation inside arithmetic functions.
- No SIMD, no threading, no aggressive optimization until Phase 7.
- Every performance claim in the README has a measured number attached.
- No third-party cryptographic or bignum code, ever.

---

## Timeline

| Phase | Duration |
|---|---|
| 0 — Setup | 2 days |
| 1 — Big integers | 1 week |
| 2 — Modular arithmetic | 4 days |
| 3 — Primality + RSA | 1 week |
| 4 — DH + SHA-256 | 1 week |
| 5 — ECC (stretch) | 1–2 weeks |
| 6 — WASM + UI | 1 week |
| 7 — Polish | 3 days |

**Total: 5–6 weeks of evenings**, including ECC.

---

## Definition of finished

- [ ] `make test` passes with vectors for every algorithm implemented
- [ ] Runs natively and as WASM from the same `core/` sources
- [ ] Four demos work in the browser with visible intermediate values
- [ ] README explains the math, the measured timings, and the limitations
- [ ] Zero warnings under `-Wall -Wextra -Werror -pedantic`
- [ ] The words "educational, not constant-time" appear prominently

---

## Non-goals

- Being a real security product
- Constant-time execution or side-channel resistance
- Networked use of any kind
- Supporting every padding scheme or curve