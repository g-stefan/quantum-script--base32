// Quantum Script Extension Base32
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Base32/Library.hpp>
#include <XYO/QuantumScript.Extension/Base32/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Base32/License.hpp>
#include <XYO/QuantumScript.Extension/Base32/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>

namespace XYO::QuantumScript::Extension::Base32 {

	static TPointer<Variable> encode(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- base32-encode\n");
#endif
		return VariableString::newVariable(XYO::Encoding::Base32::encode((arguments->index(0))->toString()));
	};

	static TPointer<Variable> decode(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- base32-decode\n");
#endif
		String result;
		if (XYO::Encoding::Base32::decode((arguments->index(0))->toString(), result)) {
			return VariableString::newVariable(result);
		};
		return Context::getValueUndefined();
	};

	static TPointer<Variable> decodeToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME
		printf("- base32-decode-to-buffer\n");
#endif
		String result;
		if (XYO::Encoding::Base32::decode((arguments->index(0))->toString(), result)) {
			return Extension::Buffer::VariableBuffer::newVariableFromString(result);
		};
		return Context::getValueUndefined();
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Base32", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {
		String info = "Base32\r\n";
		info << License::shortLicense();

		executive->setExtensionName(extensionId, "Base32");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Base32::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX("Script.requireExtension(\"Buffer\");");
		executive->compileStringX("var Base32={};");

		executive->setFunction2("Base32.encode(str)", encode);
		executive->setFunction2("Base32.decode(str)", decode);
		executive->setFunction2("Base32.decodeToBuffer(str)", decodeToBuffer);
	};

};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_BASE32_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Base32::initExecutive(executive, extensionId);
};
#endif