// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("Base32");

var checkEncoding = [
		["", ""],
		["f", "MY======"],
		["fo", "MZXQ===="],
		["foo", "MZXW6==="],
		["foob", "MZXW6YQ="],
		["fooba", "MZXW6YTB"],
		["foobar", "MZXW6YTBOI======"]
	];

var allOk = true;
for(var k of checkEncoding) {
	if(Base32.encode(k[0]) != k[1]) {
		allOk = false;
		Console.writeLn("Error-Encode: " + k[0] + " : " + k[1] + " : " + Base32.encode(k[0]));
	};
};

for(var k of checkEncoding) {
	if(Base32.decode(k[1]) != k[0]) {
		allOk = false;
		Console.writeLn("Error-Decode: " + k[1] + " : " + k[0] + " : " + Base32.decode(k[1]));
	};
};

if(allOk) {
	Console.writeLn("-> test 0001 ok");
};
