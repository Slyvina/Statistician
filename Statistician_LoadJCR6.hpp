// Lic:
// Statistician/Statistician_LoadJCR6.hpp
// Statician - Load from JCR6 (header)
// version: 23.07.31
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
#include "Statistician.hpp"
#include <JCR6_Core.hpp>

namespace Slyvina {
	namespace Statistician {

		enum class JCR6_Response{Ignore,Warn,Throw};

		/// <summary>
		/// Generally the response to when a function was set to script a stat. Since functions cannot be saved, an error tag was saved in the file was set. This enum variable serves to how to respond to that tag.
		/// </summary>
		extern JCR6_Response StatFunctionResponse;

		Party JCR6_LoadParty(JCR6::JT_Dir J, std::string dir = "");
		Party JCR6_LoadParty(std::string J, std::string dir = "");
		UParty JCR6_LoadUParty(JCR6::JT_Dir J, std::string dir = "");
		UParty JCR6_LoadUParty(std::string J, std::string dir = "");
	}
}