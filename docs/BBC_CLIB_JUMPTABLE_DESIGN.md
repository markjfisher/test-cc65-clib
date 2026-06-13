# Design: CLIB ROM jump-table / vectoring layer (Phase 5)

Status: DESIGNED + PROTOTYPED, not adopted. The prototype works in isolation but
destabilises the bbc-clib integration tests; the open issue (§6) must be resolved
before this is merged. The sideways-ROM private-workspace idea is explicitly OUT
of scope for this work.

## 1. Problem

The bbc-clib target resolves ROM-resident library symbols to their **direct,
absolute ROM code addresses** (generated into `clib_stubs.s`, e.g.
`_strlen := $9B20`). Those addresses move whenever the ROM is rebuilt, so an
application binary is bound to one exact ROM build: the README warns the `.lib`
and `.rom` must come from the same build, and any ROM change silently breaks
previously-linked applications. The goal of vectoring is to decouple application
binaries from ROM code layout.

## 2. Mechanism

Insert a **fixed-address jump table** in the ROM:

* A `JUMPTABLE` segment at a fixed base (`$8100`), 3 bytes per slot, one
  `jmp <function>` per slot.
* `clib_stubs.s` resolves vectored functions to their **slot address**
  (`base + index*3`) instead of the relocatable code address. Applications call
  the slot; the slot JMPs to the real (relocatable) body.
* Because the slot addresses are fixed and stable, a rebuilt ROM (whose function
  bodies have moved) still serves old application binaries unchanged.

Cost: one extra `JMP` (3 bytes, 3 cycles) per vectored call. ROM-internal calls
stay direct (the ROM links normally; only the app-facing `clib_stubs.s` points
through the table).

## 3. Key design decisions

* **Functions only.** ROM exports include data symbols (e.g. `_hextab`,
  `ctypemask`); a `JMP` slot is only valid for code. The table is therefore
  driven by an explicit, curated list (`src/jumptable.def`) of public function
  symbols, not by all exports. Data symbols keep direct addresses.
* **Append-only slot stability.** `jumptable.def` line order == slot index and is
  permanent: append new functions at the end, never reorder/delete; retire a slot
  by replacing the symbol with `RESERVED` (emits 3 zero bytes, keeping later
  slots fixed). This is what gives cross-version binary compatibility.
* **Shared ABI constants.** `JUMPTABLE_BASE=$8100`, `SLOT_SIZE=3` are duplicated
  (with comments) in `clib_imports.py`, `clib_stubs.py`, and `clib_rom.cfg`'s
  `JUMP_M`. They must agree.

## 4. Implementation (prototyped; reverted)

The full prototype lives in this repo's git history (the reverted Phase 5 diff).
It consisted of:

* `cc65-clib/src/jumptable.def` — curated, append-only function list (seeded with
  abs/labs/atoi/atol/itoa/ltoa/memcmp/memcpy/memmove/memset and the str* family).
* `cc65-clib/src/clib_rom.cfg` — split the single `MAIN` region into `HEADER_M`
  ($8000), `JUMP_M` ($8100), `MAIN` ($8400), each with **`fill = yes`** (see §5).
* `cc65-clib/src/clib_rom.s` — emit the included jump table into the `JUMPTABLE`
  segment.
* `cc65-clib/src/clib_imports.py` — `_write_jump_table()` reads `jumptable.def`
  and emits `jmp <sym>` per slot (RESERVED -> 3 zero bytes); errors if a listed
  symbol is not ROM-resident.
* `cc65-clib/src/clib_stubs.py` — `read_vectored_symbols()` overrides vectored
  symbols' stub addresses with `base + index*3`; data/`.lbl` keep real addresses.

Verified correct in isolation:
* `JUMPTABLE` lands at `$8100`; stubs vectored (`_abs:=$8100`, `_strlen:=$812D`,
  `_toupper:=$8148`); each slot's `jmp` targets the real function (checked
  against the map, e.g. `$812D -> jmp $9EE1` == real `_strlen`).
* A bbc-clib program that makes a direct ROM call and then a vectored
  `strlen("Hello")` printed `L5` correctly through the table.

## 5. Bug found & fixed during prototyping: ROM image padding

The first attempt put the header/title at the wrong place: ld65 packs segments
**contiguously in the output file**, so with gapped memory regions the `$8100`
table landed at file offset `$3F` and was loaded at `$803F`. Fix: `fill = yes` on
`HEADER_M`/`JUMP_M` so each region is padded to its full size in the image and
`file_offset == (address - $8000)`. This is a general requirement for any future
fixed-address ROM layout work and is worth keeping in mind regardless of the
jump table.

## 6. Open issue (why it was not adopted)

With the vectoring ROM in place, the **bbc-clib integration tests became
unstable**: several programs abended back to BASIC / hung at a vectored call,
deterministically for some programs (e.g. fileio using `strcmp`) and flakily for
others (TCONSL passed under one test but failed under another). At an observed
failure the CPU was back in the MOS idle loop with BASIC paged — i.e. the program
exited rather than completing. Curiously, a program that made a direct ROM call
*before* the vectored call worked, while one that made only local calls first did
not, which points at a ROM-paging / boot-timing interaction rather than a fault
in the table itself (the table and its targets verified correct).

This needs focused, interactive debugging (single-step a failing vectored call,
watch `$F4`/`$FE30` and the stack across `crt0` -> `main` -> slot -> body) — not
appropriate for an unattended run. Until resolved, the bbc-clib target keeps the
proven, stable **direct-address** stubs.

## 7. Recommendation / next steps

1. Reproduce the §6 failure under a debugger; determine whether the CLIB ROM is
   actually paged at the vectored `jsr`, and whether `crt0`/MOS interaction
   un-pages it after only-local calls.
2. If it is a paging-state issue, consider making each jump-table slot
   self-paging (select the CLIB ROM, JMP, restore) — but that needs the ROM bank
   number, i.e. the private-workspace question that is out of scope here.
3. Once stable, add a forward-compatibility test: build an app, rebuild the ROM
   with perturbed code size (table unchanged), and rerun the *old* binary against
   the new ROM to prove decoupling.
