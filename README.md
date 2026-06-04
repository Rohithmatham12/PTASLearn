# PTASLearn

PTASLearn is a compact C++ learning project for computer architecture and silicon test concepts. It is intentionally small enough to explain in an interview, but concrete enough that every resume bullet has running code behind it.

## What It Builds

- **x86 decoder**: decodes a practical subset of x86 machine code, including `NOP`, `RET`, register `MOV`, immediate `MOV`, and short `JMP`.
- **MESI cache simulator**: models cache-line state transitions across multiple cores for `read`, `write`, and `evict` operations.
- **Scan-chain coverage tracker**: records observed scan vectors, per-flip-flop toggle coverage, stuck-at-0/stuck-at-1 coverage, and transition coverage.
- **ATPG-inspired generator**: greedily generates scan vectors that improve uncovered stuck-at and transition faults.

## Quick Start

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/ptaslearn_cli
```

## Why This Is Interview-Defensible

The project connects real Intel-adjacent vocabulary to implementations:

- DFT/scan chains are represented as serializable vectors of flip-flop values.
- Coverage-driven verification is represented by coverage metrics that improve as vectors are observed.
- ATPG is represented by a deterministic heuristic that chooses vectors based on uncovered faults.
- MESI shows cache-coherence behavior for multicore reads/writes.
- The x86 decoder demonstrates instruction-byte parsing and operand extraction.

## Example Output

```text
x86 decode:
  0x90              nop
  0xb8 0x34 0x12... mov eax, 0x1234

MESI:
  core0 read 0x1000 -> E
  core1 read 0x1000 -> S/S
  core0 write 0x1000 -> M/I

Scan coverage:
  vectors=6 stuck-at=100% transition=100%
```

## Project Layout

```text
include/ptaslearn/  Public C++ headers
src/                Implementations and CLI demo
tests/              Dependency-free test harness
docs/               Interview notes and design details
```
