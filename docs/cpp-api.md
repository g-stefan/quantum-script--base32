# C++ API

For hosts that embed Quantum Script and for maintainers of the extension.
Read the `quantum-script` repository's `docs/embedding.md` and
`docs/writing-extensions.md` first: native functions, `Variable` and
`TPointer` work the same way here.

## Headers and namespace

```cpp
#include <XYO/QuantumScript.Extension/Base32.hpp>   // Library.hpp

using namespace XYO::QuantumScript;
```

Namespace: `XYO::QuantumScript::Extension::Base32`. Export macro:
`XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT`:

| Define | Effect |
|--------|--------|
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE32_INTERNAL` (or `QUANTUM_SCRIPT__BASE32_INTERNAL`, set by fabricare while building the DLL) | export macro = `XYO_PLATFORM_LIBRARY_EXPORT` |
| none | export macro = `XYO_PLATFORM_LIBRARY_IMPORT` (consumers of the DLL) |
| `XYO_QUANTUMSCRIPT_EXTENSION_BASE32_LIBRARY` | export macro empty, no `quantumScriptExtension` entry point (sources compiled into another library) |
| `XYO_PLATFORM_COMPILE_STATIC` (static platforms) | `XYO_PLATFORM_LIBRARY_EXPORT` / `IMPORT` are empty |

The `quantumScriptExtension` entry point is compiled only when
`XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` is defined and
`XYO_QUANTUMSCRIPT_EXTENSION_BASE32_LIBRARY` is not.

## Registering the extension

```cpp
void Extension::Base32::registerInternalExtension(Executive *executive);
void Extension::Base32::initExecutive(Executive *executive, void *extensionId);
```

- `registerInternalExtension` registers `"Base32"` as an internal extension;
  call it from the host's init callback, together with
  `Extension::Buffer::registerInternalExtension` (see
  [Getting started](getting-started.md#4-register-it-in-a-c-host)).
- `initExecutive` is the extension's init function, run by the engine when a
  script first requires `Base32` in a thread. It sets the extension name,
  info (license text), version and public flag, then:

  ```cpp
  executive->compileStringX("Script.requireExtension(\"Buffer\");");
  executive->compileStringX("var Base32={};");
  executive->setFunction2("Base32.encode(str)", encode);
  executive->setFunction2("Base32.decode(str)", decode);
  executive->setFunction2("Base32.decodeToBuffer(str)", decodeToBuffer);
  ```

  Do not call it directly.
- The DLL build also exports
  `extern "C" void quantumScriptExtension(Executive *, void *)`, which
  forwards to `initExecutive`; it is what `Script.requireExtension` looks up
  in `quantum-script--base32.dll`.

## The native functions

All three are `static` in `Base32/Library.cpp` and wrap the codec in
`xyo-encoding`:

| Script function | C++ | Returns |
|-----------------|-----|---------|
| `Base32.encode(str)` | `XYO::Encoding::Base32::encode(arguments->index(0)->toString())` | `VariableString` |
| `Base32.decode(str)` | `XYO::Encoding::Base32::decode(text, result)` | `VariableString`, or `Context::getValueUndefined()` on failure |
| `Base32.decodeToBuffer(str)` | same decode | `Extension::Buffer::VariableBuffer::newVariableFromString(result)`, or undefined |

With `XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` defined each function prints a trace
line (`- base32-encode`, ...).

## Using the codec without the script engine

C++ code that only needs Base32 conversion should call `xyo-encoding`
directly instead of going through a script:

```cpp
#include <XYO/Encoding.hpp>

XYO::Encoding::String text = XYO::Encoding::Base32::encode("foo");   // "MZXW6==="

XYO::Encoding::String bytes;
if (XYO::Encoding::Base32::decode(text, bytes)) {
	// bytes == "foo"
};
```

| Function | Behavior |
|----------|----------|
| `String encode(const String &toEncode)` | RFC 4648 alphabet `A-Z 2-7`, `=` padded to a multiple of 8 |
| `bool decode(const String &toDecode, String &out)` | `false` (and `out` unchanged) when the length is not a multiple of 8, a character is outside `A-Z 2-7 =` (lowercase included), the padding count is not 0 / 1 / 3 / 4 / 6, or padding appears before the last group; decoding uses a 256 entry reverse lookup table |

## Notes for maintainers

- New functions: register them in `initExecutive` with
  `executive->setFunction2("Base32.name(args)", name)`, then update
  `README.md`, `docs/script-api.md`, `docs/reference.md`, the skill in
  `.claude/skills/quantum-script--base32/` and `test/test.01.js`.
- Keep the API in step with `quantum-script--base16` and
  `quantum-script--base64`, which expose the same three functions.
- Validation rules live in `xyo-encoding` (`XYO/Encoding/Base32.cpp`); a
  change there (for example accepting lowercase) changes this extension's
  behavior, so update `docs/script-api.md` and the skill with it.
- `test/test.01.js` checks the RFC 4648 test vectors (`""`, `"f"`, ...,
  `"foobar"`) in both directions; run it with `fabricare test`.
