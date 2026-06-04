# PTASLearn Interview Notes

## Scan Chain Coverage Tracker

The scan-chain model treats a scan vector as the values shifted into a chain of flip-flops. The tracker records every observed vector and updates three coverage families:

- Toggle coverage: each flip-flop has been seen as both 0 and 1.
- Stuck-at coverage: stuck-at-0 is detected when the bit is driven to 1; stuck-at-1 is detected when the bit is driven to 0.
- Transition coverage: each flip-flop has seen both 0-to-1 and 1-to-0 transitions between adjacent vectors.

The implementation is in `src/scan_chain.cpp`. It keeps per-bit boolean arrays rather than storing a complex fault database, which makes the metrics easy to inspect and explain.

## ATPG-Inspired Test Generation

Traditional ATPG tries to activate a fault and propagate the effect to an observable point. PTASLearn uses the same spirit in a scan-chain-only setting:

- For stuck-at faults, generate a vector that drives the target bit to the opposite value.
- For transition faults, generate a pair of vectors that force a rising or falling transition on the target bit.
- Start with all-zero and all-one vectors to quickly cover stuck-at faults across the whole chain.

The implementation is intentionally deterministic. That makes tests stable and makes the generated sequence easy to whiteboard.

## MESI Cache Simulator

The MESI simulator tracks one state per core per cache line:

- `I`: line is invalid in that core.
- `S`: line may be shared by multiple cores.
- `E`: line is clean and owned by one core.
- `M`: line is dirty and owned by one core.

Reads move a line to `E` if no other core has it, or `S` if another core has a valid copy. Writes invalidate other cores and move the writer to `M`.

## x86 Decoder

The decoder is not a full x86 implementation. It deliberately supports a defensible subset:

- `0x90`: `nop`
- `0xC3`: `ret`
- `0xB8` through `0xBF`: `mov r32, imm32`
- `0x89` and `0x8B` with register-direct ModR/M bytes: register-to-register `mov`
- `0xEB`: short relative jump

The important interview point is byte-level parsing: opcode dispatch, little-endian immediate extraction, ModR/M field decoding, and relative target calculation.

## Golden Reference Model

`GoldenCpuModel` is the software reference used for validation. It decodes bytes, executes the supported instruction subset, updates architectural registers, and computes flags for ALU operations. A silicon-validation style test can run generated content through this model and compare observed behavior against expected architectural state.

## Branch Predictor State Machine

`TwoBitBranchPredictor` models the classic saturating counter predictor:

- Strongly-not-taken
- Weakly-not-taken
- Weakly-taken
- Strongly-taken

Each observed branch outcome pushes the counter toward taken or not-taken. This gives branch-heavy synthetic content a concrete state machine to cover.

## Synthetic OS-Like Content

`python/synthetic_os.py` emits structured workloads for cache, branch, and mixed integer behavior. The goal is not to boot an OS; it is to generate targeted instruction and memory-access patterns analogous to synthetic validation content.
