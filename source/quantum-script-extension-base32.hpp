//
// Quantum Script Extension Base32
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE32_HPP
#define QUANTUM_SCRIPT_EXTENSION_BASE32_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE32__EXPORT_HPP
#include "quantum-script-extension-base32--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE32_COPYRIGHT_HPP
#include "quantum-script-extension-base32-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE32_LICENSE_HPP
#include "quantum-script-extension-base32-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE32_VERSION_HPP
#include "quantum-script-extension-base32-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Base32 {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_BASE32_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_BASE32_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

