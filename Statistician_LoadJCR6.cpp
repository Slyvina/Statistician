// Lic:
// Statistician/Statistician_LoadJCR6.cpp
// Statician - Load from JCR6
// version: 23.11.21
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

#include "Statistician_LoadJCR6.hpp"

#define SCHK(nid,what) if (!nid.size()) throw std::runtime_error(TrSPrintF("Data for %s, but no id has been set! Statician data could be corrupted!",what))

using namespace Slyvina;
using namespace Units;
using namespace JCR6;

namespace Slyvina {
	namespace Statistician {

		JCR6_Response StatFunctionResponse{ JCR6_Response::Ignore };

#pragma region "Read Loader"
		inline byte GetB(Bank Bnk, byte& b) { b = Bnk->ReadByte(b); return b; }

		static void JCR6_LoadParty(_Party* P, JT_Dir J, std::string dir=""){
			dir=ChReplace(dir,'\\','/');
			if (!Suffixed(dir, "/")) dir += "/";
			// Characters
			auto CharList{ J->Directory(dir + "Characters",false) };
			for (auto ChF : *CharList) {
				auto ChT{ StripDir(ChF) };
				auto BT{ J->B(ChF) };
				auto ChD{ P->NewChar(ChT) };
				byte MTag{ 0xff };
				if (BT->ReadByte() != 8) throw std::runtime_error("Only 6bit command tags supported in character stat data");
				while (GetB(BT, MTag)) {
					switch (MTag) {
					case 0: goto ChEindLoop; // Should NEVER happen, but just in case!
					case 1:
						// Character's name
						ChD->Name = BT->ReadString();
						break;
					case 2: {
						// Statistic
						byte STag{ 0xff };
						std::string STID{""};
						Stat StD{ nullptr };
						while (GetB(BT, STag)) {
							//printf("DEBUG> Stat tag: %d @%x\n", STag, (int)BT->Position() - 1);
							//std::cout << "Char:" << ChT << "; Pos:" << BT->Position()<<"/"<<TrSPrintF("%x",BT->Position()) << " >> " << (int)STag << " STID : " << STID << std::endl;
							switch (STag) {
							case 0:
								// Should never happen, but just in case!
								throw std::runtime_error("Statistic internal error! Stat-Null-Tag! Please report!");
							case 1:
								STID = BT->ReadString();
								if (STID == "") {
									std::cout << "\7WARNING!!!! Empty string received as statistic data! File possibly corrupted. Trying to void this!\n";
									static size_t errcount{0};
									STID = TrSPrintF("*ERROR*%d*", errcount++);
								}
								StD = ChD->Statistic(STID);
								break;
							case 2:
								SCHK(STID, "Stat");
								StD->Base = BT->ReadInt64();
								break;
							case 4:
								SCHK(STID, "Stat");
								if (BT->ReadByte()) StD->Mini(BT->ReadInt64());
								break;
							case 5:
								SCHK(STID, "Stat");
								if (BT->ReadByte()) StD->Maxi(BT->ReadInt64());
								break;
							case 6:
								SCHK(STID, "Stat");
								{
									auto ModTag{ BT->ReadString() };
									(*StD)[ModTag] = BT->ReadInt64();
								}
								break;
							case 7:
								switch (StatFunctionResponse) {
								case JCR6_Response::Ignore: break;
								case JCR6_Response::Warn: std::cout << "\07\x1b[31mWARNING!\x1b[0m\tFunction tied to stat, but that could not be loaded\n"; break;
								case JCR6_Response::Throw: throw std::runtime_error("Function tied to stat, but that could not be loaded");
								}
								break;
							case 8:
								SCHK(STID, "Stat");
								StD->StatScriptScript = BT->ReadString();
								break;
							default:
								throw std::runtime_error(TrSPrintF("Unknown statistic command tag in character %s (%d @%x)! Either the statistic data is corrupted or meant for a higher version of Statistician.",ChT.c_str(),STag,(int)BT->Position()-1));
							}
						}
					} break;
					case 3:
						// Data
					{
						auto key{ BT->ReadString() };
						ChD->Data[key] = BT->ReadString();
					} break;
					case 4:
						// List
					{
						auto listname{ BT->ReadString() };
						auto size{ BT->ReadUInt64() };
						for (uint64 i = 0; i < size; ++i) ChD->ListAdd(listname, BT->ReadString());
					} break;
					case 5: {
						// Points
						byte PTag{ 0xff };
						std::string PTID{""};
						Point PT{ nullptr };
						while (GetB(BT, PTag)) {
							switch (PTag) {
							case 0:
								// Should never happen, but just in case!
								throw std::runtime_error("Statistic internal error! Point-Null-Tag! Please report!");
							case 1:
								PTID = BT->ReadString();
								PT = ChD->GetPoints(PTID);
								break;
							case 2:
								SCHK(PTID, "points");
								PT->Max(BT->ReadInt64());
								break;
							case 3:
								SCHK(PTID, "points");
								PT->Min(BT->ReadInt64());
								break;
							case 4:
								SCHK(PTID, "points");
								PT->Have(BT->ReadInt64());
								break;
							case 5:
								SCHK(PTID, "points");
								PT->MaxCopy = BT->ReadString();
								break;
							case 6:
								SCHK(PTID, "points");
								PT->MinCopy = BT->ReadString();
								break;
							default:
								throw std::runtime_error(TrSPrintF("Unknown points command tag in character %s (%d)! Either the statistic data is corrupted or meant for a higher version of Statistician.", ChT.c_str(), PTag));
							}
						}
					} break;
					default:
						throw std::runtime_error(TrSPrintF("Unknown main command tag for character %s (%d). Either the character data is corrupted or you loaded a file for a later version of Statician.",ChT.c_str(),MTag));
					}
				} ChEindLoop:;
				
			}
			// Party (should be after loading characters to prevent conflicts)
			auto BT = J->B(dir + "Party");
			auto ps{ BT->ReadUInt32() };
			if (ps) {
				P->Size(ps);
				for (uint32 i = 0; i < P->Size(); i++) P->AddToParty(BT->ReadString());
			}


			// TODO: Load links and apply them
		}
#pragma endregion

#pragma region "Chain link functions"
		Party JCR6_LoadParty(JCR6::JT_Dir J, std::string dir) {
			auto ret{ new _Party() };
			JCR6_LoadParty(ret, J, dir);
			return std::shared_ptr<_Party>(ret);
		}

		Party JCR6_LoadParty(std::string J, std::string dir) {
			auto ret{ new _Party() };
			auto TJ{ JCR6_Dir(J) };
			JCR6_LoadParty(ret, TJ, dir);
			return std::shared_ptr<_Party>(ret);
		}

		UParty JCR6_LoadUParty(JCR6::JT_Dir J, std::string dir) {
			auto ret{ new _Party() };
			JCR6_LoadParty(ret, J, dir);
			return std::unique_ptr<_Party>(ret);
		}

		UParty JCR6_LoadUParty(std::string J, std::string dir) {
			auto ret{ new _Party() };
			auto TJ{ JCR6_Dir(J) };
			JCR6_LoadParty(ret, TJ, dir);
			return std::unique_ptr<_Party>(ret);
		}

#pragma endregion

	}
}