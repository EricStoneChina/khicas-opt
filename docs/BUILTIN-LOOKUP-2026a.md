# Builtin lexer lookup: constant reference table

Replace only `find_or_make_symbol` using `/tmp/khicas-builtin-lookup-function.cc` (SHA256 `dfb7ad843048f1901c7a4329bd5bc4302b8a1f04b4fcf5121fc442d90e3e338c`). `/tmp/khicas-builtin-lookup-kglobal.cc` is a review copy, not intended to overwrite concurrent production changes. No registration or generated headers change.

The existing automatic array materializes 906 unary-function pointers on every successful builtin name lookup. The candidate uses a local aggregate type with one `const unary_function_ptr * const &` member and a `static const` array initialized from the same `static_lexer_.h`. Each entry binds a reference to the corresponding `at_*` pointer variable; the array therefore contains link-time addresses of variables with static storage duration. It reads the selected pointer only when a lookup succeeds. Both pointee and reference are immutable.

This is deliberately different from a `static const unary_function_ptr *const` value array, whose initializers would read extern pointer variables and can require dynamic initialization. The reference aggregate has no constructor, does not read the pointers during initialization, creates no first-call initialization guard, and allocates no writable table. It requires neither startup order between translation units nor a thread-safe lazy-static implementation. Reentrance cannot observe a partially initialized pointer table. The selected `at_*` pointer is read at the same call-time phase as the original automatic table, then passed to the unchanged `gen(const unary_function_ptr*)` path. The NSPIRE warning about replacing this conversion with `_FUNC_` integer reinterpretation remains respected. The quoted-function low-bit adjustment and token calculation are unchanged.

The `string ts(s)` temporary was never mutated. Both read-only table searches now use `s.c_str()` while the const input reference is alive. All return paths, symbol creation and locking remain unchanged.

## SH4 compiler and link evidence

Both complete kglobal translation units used the optimized build's existing SH4 flags: `-Os -mb -m4a-nofpu -mhitachi -std=c++11 -fno-threadsafe-statics -ffunction-sections -fdata-sections -fstack-usage`, with the same other definitions and libraries. Compilation touched only /tmp objects. Full ELF links reused the same existing object files and linker script, substituting only the before/after kglobal object.

| Metric | Before | After |
|---|---:|---:|
| find_or_make_symbol stack frame | 3760 B | 96 B |
| Function text including literal pools | 9740 B | 700 B |
| New lookup rodata | 0 B | 3624 B |
| Full linked ROM | 2057940 B | 2052524 B |
| Full linked static RAM | 424620 B | 424620 B |
| Full linked AC2 | 2406692 B | 2406692 B |

Net linked ROM saving is 5416 bytes and per-call frame saving is 3664 bytes. Both object files have identical total writable data/bss (507 bytes), identical 20-byte translation-unit startup function, and no lookup initialization-guard symbol. The new table is an `r` symbol in `.rodata`, 906 four-byte references. Object/source/ELF/linker hashes and complete section/resource reports are in `docs/benchmarks/builtin-lookup-target-2026a.json` and `docs/benchmarks/builtin-lookup-linked-2026a.json`. Reproduction scripts: `/tmp/build-khicas-builtin-lookup.py`, `/tmp/link-khicas-builtin-lookup.py`.

## Semantic test scope

`python3 tests/run-builtin-lookup.py --helpers /tmp/khicas-builtin-lookup-function.cc --output docs/benchmarks/builtin-lookup-harness-2026a.json` compiles both complete lookup function bodies, with the actual repository builtin metadata and pointer-name tables. It supplies instrumented mock gen/pointer objects and three integer-keyword fixtures, independent of host Giac's already-compiled lexer. All 900 non-release and 906 release names retain their expected pointer identity (including aliases), quoted bit, parser token and index status. First candidate invocation reenters lookup for `evalf` during `sin` pointer construction, then verifies both results. Keyword and new/existing symbol branches are compared. An instrumented long builtin name needs one std::string allocation before and zero after.

These checks are not a device runtime or NSPIRE reboot test. They establish source behavior and target resource changes; the existing integral host harness links the system kglobal and cannot establish execution performance of this change. No calculator timing percentage is inferred from these results.
