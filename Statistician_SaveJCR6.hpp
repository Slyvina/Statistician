// Lic:
// Statistician/Statistician_SaveJCR6.hpp
// Statistician - Save to JCR6 (header)
// version: 23.07.28
// Copyright (C) 2023 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
#pragma once
#include <JCR6_Core.hpp>
#include <JCR6_Write.hpp>
#include "Statistician.hpp"

namespace Slyvina {
	namespace Statistician {

		void StatSaveJCR6(_Party* P, JCR6::JT_Create D, std::string dir="", std::string storage = "Store", bool asblock = true);
		void StatSaveJCR6(_Party* P,std::string File, std::string dir="", std::string storage = "Store", bool asblock = true);

		inline void StatSaveJCR6(Party P, JCR6::JT_Create D, std::string dir = "", std::string storage = "Store", bool asblock = true) { StatSaveJCR6(P.get(),D,dir,storage,asblock); }
		inline void StatSaveJCR6(Party P, std::string File, std::string dir = "", std::string storage = "Store", bool asblock = true) { StatSaveJCR6(P.get(), File, dir, storage, asblock); }
		inline void StatSaveJCR6(UParty P, JCR6::JT_Create D, std::string dir = "", std::string storage = "Store", bool asblock = true) { StatSaveJCR6(P.get(), D, dir, storage, asblock); }
		inline void StatSaveJCR6(UParty P, std::string File, std::string dir = "", std::string storage = "Store", bool asblock = true) { StatSaveJCR6(P.get(), File, dir, storage, asblock); }

	}
}