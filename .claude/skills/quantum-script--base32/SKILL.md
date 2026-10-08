---
name: quantum-script--base32
description: >-
  How to use the Quantum Script Base32 extension (quantum-script--base32),
  the RFC 4648 Base32 codec loaded with Script.requireExtension("Base32"):
  Base32.encode(str) (bytes -> uppercase A-Z 2-7, "=" padded to a multiple
  of 8, never fails, non-strings are encoded as their text: encode(255) is
  "GI2TK==="), Base32.decode(str) (strict: length a multiple of 8, uppercase
  only, padding 0/1/3/4/6 "=" in the last group only, otherwise undefined;
  returns a String of bytes) and Base32.decodeToBuffer(str) (same, returns a
  Buffer); normalizing lowercase / spaced / unpadded input such as TOTP
  secrets before decoding; testing failure with Script.isUndefined
  (decode("") is "" which is falsy); Base16 / Base64 siblings; which hosts
  have it (quantum-script, magnet; NOT fabricare build scripts); the C++
  side (registerInternalExtension together with Buffer, initExecutive,
  quantumScriptExtension entry point, XYO::Encoding::Base32). Use when
  writing or reviewing Quantum Script code that converts bytes to / from
  Base32, C++ code that includes <XYO/QuantumScript.Extension/Base32.hpp>, a
  fabricare.json depending on "quantum-script--base32", or when working
  inside the quantum-script--base32 repository.
---

# quantum-script--base32

Base32 (RFC 4648 section 6) codec extension of Quantum Script (see the
`quantum-script` skill for the language and its differences from
JavaScript, and the `quantum-script--buffer` skill for the `Buffer` type;
their rules apply). Purpose: **turn binary data into single-case text and
back** — for file names on case-insensitive systems, DNS labels, TOTP / HOTP
secrets, recovery and license keys, URL identifiers — with strict
validation on input. Output is 1.6x the input (Base16 2x, Base64 ~1.33x).

Full documentation: `docs/` in the quantum-script--base32 repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--base32\docs` on this machine):
README (purpose), getting-started (build, load, hosts, C++ registration),
**script-api** (format, exact validation, argument conversion, recipes),
cpp-api, reference. When in doubt read
`source/XYO/QuantumScript.Extension/Base32/Library.cpp` (~75 lines) and
`XYO::Encoding::Base32` (`source/XYO/Encoding/Base32.cpp`) in the
xyo-encoding repository.

## Script API

```javascript
Script.requireExtension("Base32");          // also loads Buffer

Base32.encode("foo");                       // "MZXW6==="  uppercase A-Z 2-7, "=" padded, never fails
Base32.encode(buffer);                      // Base32 of the first buffer.length bytes
Base32.decode("MZXW6===");                  // "foo"       String of bytes, or undefined
Base32.decodeToBuffer("AD7RA===");          // Buffer 00 ff 10 (size == length == 3), or undefined
```

Padding by bytes in the last 5-byte group: 1 → `======`, 2 → `====`,
3 → `===`, 4 → `=`, 5 → none. Length for `n` bytes: `Math.ceil(n / 5) * 8`.

## Hard rules

1. **Decoding is strict, failure is `undefined`.** Valid input is exactly
   what `encode` produces: length a multiple of 8, **uppercase** `A-Z 2-7`,
   then 0, 1, 3, 4 or 6 `=`, padding only in the last group of 8. Lowercase,
   unpadded text, spaces, newlines, `-`, `0 1 8 9`, two `=`, letters after
   `=`, or two padded values concatenated → `undefined`, no exception, no
   partial result.
2. **Normalize foreign input first** (authenticator apps show
   `jbsw y3dp ehpk 3pxp`): `toUpperCaseASCII()` (there is no
   `toUpperCase`), `replace(" ", "")` (replaces all), strip `=`, then re-pad
   to a multiple of 8; a remainder of 1, 3 or 6 characters is invalid.
3. **Test failure with `Script.isUndefined(r)`**, never `!r`:
   `Base32.decode("")` is `""` and `Base32.decodeToBuffer("")` is an empty
   Buffer — both falsy but valid.
4. **The argument is converted with `toString` first.** `encode(255)` is
   `"GI2TK==="` (the text "255"); `encode()` encodes the text
   `"undefined"`; `decode()` is `undefined` (9 characters). To encode raw
   bytes, use a Buffer: `var b = Buffer(1); b.setU8(0, 255); Base32.encode(b);`
   → `"74======"`. String literals cannot hold `\0`; use
   `Buffer.fromHex("610062")`.
5. **Bytes, not characters.** `Base32.encode("ă")` is `"YSBQ===="` (UTF-8);
   `decode` keeps zero bytes and does not validate UTF-8. Use
   `decodeToBuffer` when the result is binary.
6. **Unused trailing bits are not checked**: `"MZ======"` and `"MY======"`
   both decode to `"f"`. Compare decoded bytes, not Base32 text, when
   checking equality of foreign input.
7. **Not available in fabricare build scripts.** fabricare is a static host
   without `Base32`; `requireExtension("Base32")` throws
   `Unable to open "Base32"` there. Use hex (`Buffer.fromString(s).toHex()`
   / `Buffer.fromHex(h)`) instead. Available in the `quantum-script`
   interpreter (DLL), `magnet`, and hosts that register it.
8. `JSON.encode` turns a Buffer into `null`: store `Base32.encode(buffer)`
   and restore with `Base32.decodeToBuffer`.
9. Same three functions exist in `Base16` and `Base64`; keep usage
   symmetric when switching codecs (Base16 decode is case-insensitive and
   has no padding — Base32 is neither).
10. One engine per thread: each thread requires `Base32` itself; the
    functions are stateless.

## Recipes

```javascript
function decodeLoose(text) {                            // TOTP secrets, user input
	text = text.toUpperCaseASCII().replace(" ", "").replace("-", "").replace("=", "");
	var rest = text.length % 8;
	if (rest == 1 || rest == 3 || rest == 6) { return undefined; };
	if (rest) { text += "========".substring(0, 8 - rest); };
	return Base32.decodeToBuffer(text);
};
var name = Base32.encode(SHA256.hashToBuffer(data)).replace("=", "");   // 52 char file-safe id
var key = Base32.decodeToBuffer(text);                  // validate a 10 byte key
if (Script.isUndefined(key) || key.length != 10) { throw "expected a Base32 key of 10 bytes"; };
JSON.encode({key: Base32.encode(key)});                 // binary in JSON
```

## C++

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/Base32.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                 // host init callback
	Extension::Buffer::registerInternalExtension(executive);  // Base32 requires Buffer
	Extension::Base32::registerInternalExtension(executive);  // scripts still requireExtension("Base32")
};

// without the script engine:
XYO::Encoding::String text = XYO::Encoding::Base32::encode(bytes);   // "=" padded
XYO::Encoding::String out;
bool ok = XYO::Encoding::Base32::decode(text, out);                 // strict, uppercase only
```

- fabricare.json dependency: `"quantum-script--base32"` (`dll-or-lib`: DLL
  on dynamic platforms, static lib on `*.static` platforms). It pulls in
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer`.
- DLL entry point `extern "C" quantumScriptExtension(Executive *, void *)`
  only with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` and without
  `XYO_QUANTUMSCRIPT_EXTENSION_BASE32_LIBRARY`. Static hosts must register
  the extension as internal.

## Working in this repository

- Build: `fabricare make`, `fabricare test` (runs `test/test.01`, which
  registers Console, Buffer and Base32 as internal extensions and checks the
  RFC 4648 vectors in `test/test.01.js`; run `make` first),
  `fabricare install` (see the `fabricare` skill). `quantum-script`,
  `quantum-script--console` and `quantum-script--buffer` must be installed
  first. With the SDK installed, a script can also be run directly:
  `quantum-script test/test.01.js` (uses the installed DLL).
- Native functions live in `Base32/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`
  and are registered in `initExecutive` with
  `executive->setFunction2("Base32.name(args)", name)`.
- Validation rules come from `xyo-encoding`; if they change there, update
  `docs/script-api.md`, `docs/reference.md` and this skill.
- New functions: update `README.md`, `docs/script-api.md`,
  `docs/reference.md`, `test/test.01.js` and this skill; keep the API in
  step with quantum-script--base16 / --base64.
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/` and `.claude/` (see `.reuse/dep5`).
