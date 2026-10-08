# Quantum Script Extension Base32

Quantum Script extension
- Base32 (RFC 4648) encoding of strings and buffers:
`Base32.encode` writes every 5 bytes as 8 characters from `A-Z 2-7`,
padded with `=`.
- Strict decoding back to a `String` or a `Buffer`: `Base32.decode`,
`Base32.decodeToBuffer` return `undefined` on invalid input.
- For putting binary data into single-case text: file names, DNS labels,
one-time password (TOTP) secrets, keys people read and type.

```javascript
Script.requireExtension("Base32");

Base32;
Base32.encode(str);
Base32.decode(str);
Base32.decodeToBuffer(str);
```

Built on `quantum-script` and `quantum-script--buffer`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, load from a script, hosts, register in a C++ host
- [Script API](docs/script-api.md) - every function: format, exact validation, edge cases, recipes
- [C++ API](docs/cpp-api.md) - registration, DLL entry point, the codec in `xyo-encoding`
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--base32](.claude/skills/quantum-script--base32/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
