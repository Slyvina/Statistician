// Lic:
// Statistician/Statistician_SaveJCR6.cpp
// Statistician - Save to JCR6
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
#include "Statistician_SaveJCR6.hpp"

#define SSSJNE(A) if (asblock) JCS = Block->nb(dir+A, "Statician", "Set up with Statician"); else JCS = D->nb(dir+A, storage, "Statistician", "Set up with Statistician");

namespace Slyvina {
	namespace Statistician {
		void StatSaveJCR6(_Party* P, JCR6::JT_Create D, std::string dir, std::string storage, bool asblock) {
			if (storage == "") storage = "Store";
			asblock = asblock && storage != "Store";
			dir = ChReplace(dir, '\\', '/'); while (dir[0] == '/') dir = dir.substr(1); if (Right(dir, 1) != "/") dir += "/";
			JCR6::JT_CreateBlock Block{nullptr};
			JCR6::JT_CreateStream JCS{nullptr};
			if (asblock) Block = D->AddBlock(storage);
			SSSJNE("Party");
			JCS->WriteUInt32(P->Size());
			for (uint32 i = 0; i < P->Size(); i++) JCS->Write((*P)[i]);
			auto CL{ P->CharList() };
			for (auto CHID : *CL) {
				auto CH = (*P)[CHID];
				SSSJNE("Characters/" + CHID);
				JCS->WriteByte(8);
				// Name
				JCS->WriteByte(1); JCS->Write(CH->Name);
				// Statistics
				auto IS{ CH->StatList() };
				for (auto STID : *IS) {
					auto ST{ CH->Statistic(STID) };
					if (Prefixed(STID, "*ERROR*") || STID=="") {
						std::cout << "\7WARNING! Error tag or emtpy tag for stat encountered. Ignoreing for save!\n";
					} else {
						JCS->WriteByte(2);
						JCS->WriteByte(1); JCS->Write(STID);
						JCS->WriteByte(2); JCS->Write(ST->Base);

					JCS->WriteByte(4); JCS->Write((byte)ST->UseMini()); if (ST->UseMini()) JCS->Write(ST->Mini());
					JCS->WriteByte(5); JCS->Write((byte)ST->UseMaxi()); if (ST->UseMaxi()) JCS->Write(ST->Maxi());
					for (auto m : *ST->ListModifiers()) {
						JCS->WriteByte(6);
						JCS->Write(m);
						JCS->Write((*ST)[m]);
					}
				}

				// Data
				IS = CH->DataList();
				for (auto DTID : *IS) {
					JCS->WriteByte(3);
					JCS->Write(DTID);
					JCS->Write(CH->Data[DTID]);
				}

				// List
				IS = CH->ListList();
				for (auto LSID : *IS) {
					JCS->WriteByte(4);
					JCS->Write(LSID);
					auto& L{ *CH->GetList(LSID)->GetList() };
					JCS->WriteUInt64(L.size());
					for (auto& E : L) JCS->Write(E);
				}

				// Points
				IS = CH->PointsList();
				for (auto PTID : *IS) {
					auto PT{ CH->GetPoints(PTID) };
					JCS->WriteByte(5);
					JCS->WriteByte(1); JCS->Write(PTID);
					JCS->WriteByte(2); JCS->Write(PT->Max());
					JCS->WriteByte(3); JCS->Write(PT->Min());
					JCS->WriteByte(4); JCS->Write(PT->Have());
					if (PT->MaxCopy.size()) { JCS->WriteByte(5); JCS->Write(PT->MaxCopy); }
					if (PT->MinCopy.size()) { JCS->WriteByte(6); JCS->Write(PT->MinCopy); }
					JCS->WriteByte(0);
				}

				JCS->WriteByte(0);
			}
			SSSJNE("Link");
			JCS->WriteByte(0); // TODO: Link detection and saving
			if (asblock) Block->Close();
		}

		void StatSaveJCR6(_Party* P, std::string File, std::string dir, std::string storage, bool asblock) {
			auto outp{ JCR6::CreateJCR6(File,storage) };
			StatSaveJCR6(P, outp, dir, storage, asblock);
		}
	}
}