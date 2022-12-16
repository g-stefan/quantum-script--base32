// Quantum Script Extension Base32
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BASE32_VERSION_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_BASE32_VERSION_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BASE32_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Base32/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Base32::Version {

	XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT const char *version();
	XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT const char *build();
	XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT const char *versionWithBuild();
	XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT const char *datetime();

};

#endif