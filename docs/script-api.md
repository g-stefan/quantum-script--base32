# Script API

Everything the extension defines after `Script.requireExtension("Base32")`.
`Base32` is a plain object holding three native functions; it has no
constructor and no state.

Every function takes one argument and converts it to a string with the
engine's `toString` first. A Quantum Script string is a sequence of bytes
(UTF-8 by convention, zero bytes allowed), and Base32 works on those bytes.

## The format

Base32 (RFC 4648 section 6) splits the input into groups of 5 bytes (40
bits) and writes each group as 8 characters of 5 bits each, from the
alphabet:

```
value  0-25   26-31
char   A-Z    2-7
```

The last group is completed with `=` so that the output is always a
multiple of 8 characters. The amount of padding depends on how many bytes
the last group holds:

| Bytes in last group | Characters | Padding | Example |
|---------------------|------------|---------|---------|
| 1 | 2 | `======` (6) | `"f"` → `"MY======"` |
| 2 | 4 | `====` (4) | `"fo"` → `"MZXQ===="` |
| 3 | 5 | `===` (3) | `"foo"` → `"MZXW6==="` |
| 4 | 7 | `=` (1) | `"foob"` → `"MZXW6YQ="` |
| 5 | 8 | none | `"fooba"` → `"MZXW6YTB"` |

Encoded length for `n` bytes: `Math.ceil(n / 5) * 8`.

## `Base32.encode(str)`

Returns a `String` with the bytes of `str` in Base32: uppercase `A-Z 2-7`,
padded with `=` as in the table above. It never fails.

```javascript
Base32.encode("");          // ""
Base32.encode("f");         // "MY======"
Base32.encode("foobar");    // "MZXW6YTBOI======"
Base32.encode("Hello");     // "JBSWY3DP"
Base32.encode("ă");         // "YSBQ===="  (UTF-8 bytes c4 83)
```

Argument conversion — anything that is not a string is encoded as its text
form, which is rarely what you want:

| Argument | Encoded text | Result |
|----------|--------------|--------|
| `Buffer` | its first `length` bytes | `Base32.encode(Buffer.fromHex("00ff10"))` → `"AD7RA==="` |
| number | decimal text | `Base32.encode(255)` → `"GI2TK==="` (the text `"255"`) |
| `true` / `null` | `"true"` / `"null"` | `"ORZHKZI="` / `"NZ2WY3A="` |
| missing / `undefined` | `"undefined"` | `"OVXGIZLGNFXGKZA="` — no error |
| array | elements joined with `,` | `Base32.encode([1, 2])` → `"GEWDE==="` |

To encode binary data or the value of a number as bytes, put it into a
buffer: `var b = Buffer(1); b.setU8(0, 255); Base32.encode(b);` → `"74======"`.
String literals cannot hold a zero byte (`"a\0b"` ends at the `\0`); build
such data with `Buffer.fromHex("610062")`.

## `Base32.decode(str)`

Decodes Base32 text and returns the bytes as a `String`, or **`undefined`**
when the text is not valid. Validation is strict — the input must look
exactly like `encode` output:

- the length is a **multiple of 8** (unpadded input is rejected);
- every character before the padding is **uppercase** `A-Z` or `2-7`
  (lowercase, `0`, `1`, `8`, `9`, spaces, line breaks, `-` are rejected);
- the padding is 0, 1, 3, 4 or 6 `=` characters (2, 5, 7 or 8 cannot come
  from any input length);
- after the first `=` there are only `=`, and only the **last** group of 8
  may contain padding (no concatenated padded values).

```javascript
Base32.decode("MZXW6===");            // "foo"
Base32.decode("JBSWY3DP");            // "Hello"
Base32.decode("");                    // ""  (valid, empty)
Base32.decode("mzxw6===");            // undefined: lowercase
Base32.decode("MZXW6");               // undefined: not padded to 8
Base32.decode("MZXW 6==");            // undefined: space
Base32.decode("MZXW6===\n");          // undefined: line break
Base32.decode("MZXW6Y01");            // undefined: "0" and "1" are not in the alphabet
Base32.decode("MZXW6Y==");            // undefined: 2 "=" is not a valid padding
Base32.decode("MZ=XW6==");            // undefined: letters after "="
Base32.decode("MY======MZXW6YTB");    // undefined: padding before the last group
Base32.decode("========");            // undefined
Base32.decode();                      // undefined: "undefined" has 9 characters
```

- Decoding is all or nothing: there is no partial result and no error
  message, only `undefined`.
- Zero bytes are kept: `Base32.decode("MEAGE===").length` is `3`
  (`a`, `0x00`, `b`).
- The result is not checked for UTF-8: decoding a digest gives a string of
  raw bytes. Use `decodeToBuffer` when the data is binary.
- The unused low bits of the last character are ignored, not checked:
  `"MZ======"` decodes to `"f"` just like `"MY======"`. So
  `Base32.encode(Base32.decode(t)) == t` holds for `encode` output but not
  for every accepted `t`; compare decoded bytes, not text, when that
  matters.
- **Test for failure with `Script.isUndefined(r)`** (or `r === undefined`),
  not with `!r`: a valid empty input decodes to `""`, which is falsy too.

## `Base32.decodeToBuffer(str)`

Same validation as `decode`, but returns the bytes as a **`Buffer`** (from
the `Buffer` extension), or `undefined` when the text is not valid Base32.

```javascript
var b = Base32.decodeToBuffer("AD7RA===");
typeof(b);       // "Buffer"
b.length;        // 3
b.size;          // 3
b.getU8(1);      // 255
b.toHex();       // "00ff10"

Base32.decodeToBuffer("xyz");  // undefined
Base32.decodeToBuffer("");     // empty Buffer, length 0
```

- The buffer is new: `size == length ==` number of decoded bytes.
- An empty buffer is falsy (`if (b)` tests `length > 0`): use
  `Script.isUndefined(b)` to detect invalid input here too.
- Use it to feed binary APIs: `File.writeFromBuffer`, `Socket.writeFromBuffer`,
  `Shell.filePutContentsBuffer`, `Crypt.decrypt`, `OpenSSL` functions.

## Recipes

### Normalize user input (TOTP secrets)

Authenticator apps and web sites show secrets in lowercase, in groups
separated by spaces, and without padding (`jbsw y3dp ehpk 3pxp`). Clean the
text before decoding:

```javascript
function decodeLoose(text) {
	text = text.toUpperCaseASCII().replace(" ", "").replace("-", "").replace("=", "");
	var rest = text.length % 8;
	if (rest == 1 || rest == 3 || rest == 6) {
		return undefined;   // no input length gives these
	};
	if (rest) {
		text += "========".substring(0, 8 - rest);
	};
	return Base32.decodeToBuffer(text);
};

decodeLoose("jbsw y3dp ehpk 3pxp");   // 10 byte Buffer
decodeLoose("mzxw6");                 // Buffer "foo"
```

`String.prototype.replace` replaces every occurrence. Quantum Script has
`toUpperCaseASCII()`, not `toUpperCase()`.

### File name or identifier from a digest

```javascript
Script.requireExtension("SHA256");
Script.requireExtension("Base32");

var id = Base32.encode(SHA256.hashToBuffer("release-1.0"));
// "JEU32QGDTLEXJS5C5SP74RDDHJFNTESKVEYNWSRDZDK5ZAYFR3TQ====", 56 characters
var name = id.replace("=", "");   // 52 characters, safe on case-insensitive file systems
```

To decode a name stripped this way, re-pad it as in `decodeLoose`.

### Store binary data in JSON

`JSON.encode` turns a `Buffer` into `null`; store its Base32 form instead.

```javascript
Script.requireExtension("JSON");
Script.requireExtension("Base32");

var key = Base32.decodeToBuffer("RDCACUQ=");          // 4 bytes 88 c4 01 52
var text = JSON.encode({key: Base32.encode(key)});    // {"key":"RDCACUQ="}
var back = Base32.decodeToBuffer(JSON.decode(text).key);
```

### Validate a key of a given size

```javascript
function parseKey(text, bytes) {
	var key = Base32.decodeToBuffer(text);
	if (Script.isUndefined(key) || key.length != bytes) {
		throw "expected a Base32 key of " + bytes + " bytes";
	};
	return key;
};

var key = parseKey("JBSWY3DPEHPK3PXP", 10);
```

### Write a binary file from Base32

```javascript
Script.requireExtension("Shell");
Script.requireExtension("Base32");

Shell.filePutContentsBuffer("magic.bin", Base32.decodeToBuffer("RFIE4RYNBINAU==="));
// the 8 byte PNG signature 89 50 4e 47 0d 0a 1a 0a
```

### Round trip

For any string `s`: `Base32.decode(Base32.encode(s)) == s`. For any `t`
produced by `encode`: `Base32.encode(Base32.decode(t)) == t`.

## Related extensions

| Extension | Same three functions | Output alphabet | Decoding |
|-----------|----------------------|-----------------|----------|
| `Base16` | `encode`, `decode`, `decodeToBuffer` | `0-9 A-F`, no padding | case-insensitive |
| `Base32` | `encode`, `decode`, `decodeToBuffer` | `A-Z 2-7`, `=` padding | uppercase only, padding required |
| `Base64` | `encode`, `decode`, `decodeToBuffer` | `A-Z a-z 0-9 + /`, `=` padding | case-sensitive by design |
| `Buffer` | `Buffer.fromHex(str)`, `b.toHex()` | lowercase hex | lenient (invalid digit → 0) |
