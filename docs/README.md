# Quantum Script Extension Base32 — Documentation

`quantum-script--base32` is the **Base32 encoding extension of Quantum
Script**. Loaded with `Script.requireExtension("Base32")`, it adds a `Base32`
object to scripts with three functions:

```javascript
Base32.encode(str);           // bytes  -> "JBSWY3DP" (RFC 4648 alphabet, "=" padded)
Base32.decode(str);           // Base32 -> String of bytes, undefined if invalid
Base32.decodeToBuffer(str);   // Base32 -> Buffer of bytes, undefined if invalid
```

Base32 (RFC 4648 section 6) writes every 5 bytes as 8 characters from the
alphabet `A-Z 2-7`. Its output has a single letter case and no characters
that are easily confused (`0`, `1`, `8`, `9` are not used), so it fits where
text must survive case-insensitive systems or be read and typed by people:
file names on Windows, DNS labels, one-time password (TOTP / HOTP) secrets,
license and recovery keys, identifiers in URLs.

- **Encode bytes, not characters.** The argument is converted to a string
  and its bytes are encoded: `"ă"` (UTF-8 `c4 83`) encodes to `"YSBQ===="`.
  A `Buffer` argument is encoded byte for byte.
- **Canonical output.** `encode` always writes uppercase `A-Z 2-7` and pads
  with `=` to a multiple of 8 characters.
- **Strict decoding.** `decode` accepts exactly that form: uppercase only,
  length a multiple of 8, a valid amount of `=` padding and only in the last
  group. Lowercase, missing padding, spaces or line breaks make it return
  `undefined`. There is no partial result.
- **Two output types.** `decode` returns a `String` (bytes kept as they
  are), `decodeToBuffer` a `Buffer` from the `Buffer` extension, which the
  binary APIs (`File.writeFromBuffer`, `Crypt.decrypt`, ...) expect.

```
scripts: quantum-script .js, magnet, embedding hosts, ...
quantum-script--base32    <-- this extension: Base32.encode / decode / decodeToBuffer
quantum-script--buffer    (the Buffer type returned by decodeToBuffer, loaded automatically)
quantum-script            (Executive, Variable, Context)
xyo-encoding              (XYO::Encoding::Base32::encode / decode, the codec)
xyo-system, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `Base32` gives |
|------|---------------------|
| Binary value as text that survives case-insensitive storage (file names, DNS, Windows paths) | `Base32.encode(bytes)` — one case, `A-Z 2-7` and `=` |
| Keys people read aloud or type (TOTP secrets, recovery codes) | no `0/O`, `1/I/l`, `8/B` confusion in the alphabet |
| Read Base32 produced by other tools (authenticator apps, RFC 4648 libraries) | `Base32.decode(text)` after normalizing (see [Script API](script-api.md#normalize-user-input-totp-secrets)) |
| Validate Base32 input | `decode` returns `undefined` on any invalid input |
| Get bytes for binary APIs | `Base32.decodeToBuffer(text)` returns a `Buffer` |
| Same API as the other codecs | `Base16` and `Base64` have the same three functions |

Size compared to the input: Base16 2x, **Base32 1.6x**, Base64 ~1.33x.
Base32 sits in the middle: smaller than hex, but unlike Base64 it uses one
letter case and no `+` / `/`.

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Base32");` | also loads `Buffer` |
| Encode a string | `Base32.encode("foo")` | `"MZXW6==="` |
| Encode a buffer | `Base32.encode(buffer)` | first `length` bytes |
| Decode to a string | `Base32.decode("MZXW6===")` | `"foo"`; `undefined` if invalid |
| Decode to a buffer | `Base32.decodeToBuffer("AD7RA===")` | `Buffer`, `length` 3; `undefined` if invalid |
| Check for failure | `Script.isUndefined(r)` | not `!r`: decoding `""` gives `""`, which is falsy |
| Decode lowercase / unpadded input | uppercase and re-pad first | `decode("mzxw6")` is `undefined` |
| Output length | `Math.ceil(n / 5) * 8` characters for `n` bytes | always a multiple of 8 |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, load the extension from a script, magnet and fabricare scripts, register it in a C++ host |
| [Script API](script-api.md) | Every function: argument conversion, exact validation rules, edge cases, recipes |
| [C++ API](cpp-api.md) | `registerInternalExtension`, `initExecutive`, the DLL entry point, using the codec directly from C++ |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`; the `Buffer` type in the `quantum-script--buffer` repository,
`docs/`; the codec in the `xyo-encoding` repository, `docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/Base32.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/Base32.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Base32/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                   initExecutive, registerInternalExtension,
                                                         encode / decode / decodeToBuffer
    Copyright / License / Version                        library metadata
test/test.01.cpp                                         C++ host registering Console, Buffer, Base32 as internal
test/test.01.js                                          RFC 4648 test vectors, encode and decode
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--base32/`](../.claude/skills/quantum-script--base32/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `Base32`
(Quantum Script tools, magnet scripts, other extensions).
