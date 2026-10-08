# API reference

## Script

Available after `Script.requireExtension("Base32")` (which also loads
`Buffer`).

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Base32.encode(str)` | String | bytes of `toString(str)` in RFC 4648 Base32, uppercase `A-Z 2-7`, `=` padded to a multiple of 8; never fails |
| `Base32.decode(str)` | String or `undefined` | length a multiple of 8, uppercase `A-Z 2-7`, padding 0 / 1 / 3 / 4 / 6 `=` in the last group only; otherwise `undefined` |
| `Base32.decodeToBuffer(str)` | Buffer or `undefined` | same validation as `decode`; `size == length` = decoded bytes |

### Edge cases

| Expression | Result |
|------------|--------|
| `Base32.encode("")` | `""` |
| `Base32.encode("foo")` | `"MZXW6==="` |
| `Base32.encode(buffer)` | Base32 of the first `length` bytes |
| `Base32.encode(255)` | `"GI2TK==="` (the text `"255"`) |
| `Base32.encode()` | `"OVXGIZLGNFXGKZA="` (the text `"undefined"`) |
| `Base32.decode("")` | `""` |
| `Base32.decode("mzxw6===")` | `undefined` (lowercase) |
| `Base32.decode("MZXW6")` | `undefined` (not padded) |
| `Base32.decode("MZXW6Y==")` | `undefined` (2 `=`) |
| `Base32.decode("MY======MZXW6YTB")` | `undefined` (padding before the last group) |
| `Base32.decode("MZ======")` | `"f"` (unused low bits are not checked) |
| `Base32.decode("MEAGE===").length` | `3` (zero byte kept) |
| `Base32.decodeToBuffer("")` | empty Buffer (`length` 0, falsy) |

### Errors

| Message | Cause |
|---------|-------|
| `Unable to open "Base32"` | the extension library was not found and no internal one is registered (for example in `fabricare` scripts) |

The three functions themselves never throw: invalid input to `decode` /
`decodeToBuffer` gives `undefined`.

## C++

Namespace `XYO::QuantumScript::Extension::Base32`, umbrella header
`<XYO/QuantumScript.Extension/Base32.hpp>`.

### Library (`Base32/Library.hpp`)

| Symbol | Notes |
|--------|-------|
| `void registerInternalExtension(Executive *executive)` | register `"Base32"` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | extension init, run by the engine |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point (not in static builds) |

### Metadata

| Symbol | Notes |
|--------|-------|
| `Version::version()`, `Version::build()`, `Version::versionWithBuild()`, `Version::datetime()` | from `version.json` |
| `Copyright::copyright()`, `Copyright::publisher()`, `Copyright::company()`, `Copyright::contact()` | |
| `License::license()`, `License::shortLicense()` | MIT text |

`Version`, `Copyright` and `License` exist in every XYO library: qualify them
(`Extension::Base32::Version::versionWithBuild()`).

### Build configuration

| Name | Meaning |
|------|---------|
| `quantum-script--base32` | fabricare project, `dll-or-lib` |
| `test.01` | fabricare test project, runs `test/test.01.js` |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT` | export / import macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE32_INTERNAL` | defined while building the DLL (from `QUANTUM_SCRIPT__BASE32_INTERNAL`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE32_LIBRARY` | static build: empty export macro, no DLL entry point |

### Codec (`xyo-encoding`)

| Symbol | Notes |
|--------|-------|
| `XYO::Encoding::Base32::encode(const String &)` | RFC 4648, `=` padded |
| `XYO::Encoding::Base32::decode(const String &, String &out)` | `bool`; strict, uppercase only, padding required |
