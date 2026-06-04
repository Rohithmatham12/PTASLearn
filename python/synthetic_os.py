#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
from dataclasses import asdict, dataclass


@dataclass
class Workload:
    name: str
    instructions: list[str]
    memory_addresses: list[int]
    branches: list[bool]


def cache_stress() -> Workload:
    return Workload(
        name="cache-stress",
        instructions=["mov eax, imm32", "mov ebx, eax"] * 8,
        memory_addresses=[0x1000 + i * 64 for i in range(16)],
        branches=[False, True, False, True],
    )


def branch_heavy() -> Workload:
    pattern = [True, True, False, True, False, False, True, True]
    return Workload(
        name="branch-heavy",
        instructions=["jmp rel8" if taken else "nop" for taken in pattern],
        memory_addresses=[0x2000, 0x2040],
        branches=pattern,
    )


def mixed_integer() -> Workload:
    return Workload(
        name="mixed-integer",
        instructions=["mov eax, imm32", "mov ecx, imm32", "add eax, ecx", "sub eax, ecx"],
        memory_addresses=[0x3000, 0x3004, 0x3040],
        branches=[False, False, True],
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--kind", choices=["cache", "branch", "mixed"], default="mixed")
    args = parser.parse_args()
    workload = {"cache": cache_stress, "branch": branch_heavy, "mixed": mixed_integer}[args.kind]()
    print(json.dumps(asdict(workload), indent=2))


if __name__ == "__main__":
    main()
