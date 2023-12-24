// Lic:
// Statistician/Statistician.hpp
// Statistician (header)
// version: 23.12.24
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
#include <Slyvina.hpp>
#include <memory>

namespace Slyvina {
	namespace Statistician {

		class _Party; typedef std::shared_ptr<_Party> Party; typedef std::unique_ptr<_Party> UParty;
		class _Char; typedef std::shared_ptr<_Char> Character;
		class _Stat; typedef std::shared_ptr<_Stat> Stat;
		class _List; typedef std::shared_ptr<_List> List;
		class _Data; typedef std::shared_ptr<_Data> ChData;
		class _Point; typedef std::shared_ptr<_Point> Point;

		typedef void(*StatScript)(_Stat*, std::string script);

		class _Party {
		private:
			static uint64 _cnt;
			uint64 _id;
			std::string* _PartyArray{nullptr};
			uint32 _PSize{ 0 };
			inline void _KillParty() { if (_PartyArray) delete[] _PartyArray; }
			std::map<std::string, Character> _Characters{};
		public:
			inline uint64 ID() { return _id; }
			_Party();
			~_Party();
			void Size(uint32);
			inline uint32 Size() { return _PSize; }
			uint32 PartyMembers();
			Character Ch(uint32 idx);
			Character Ch(std::string);
			Character NewChar(bool, std::string, std::string = "");
			inline Character NewChar(std::string a, std::string b = "") { return NewChar(false, a, b); }
			void AddToParty(std::string cid, bool ignorewhenfull = true);
			std::string& PIndex(uint32 idx);
			inline std::string& operator[](uint32 idx) { return PIndex(idx); }
			inline Character operator[](std::string chid) { return Ch(chid); }
			void FindInParty(bool& success, uint32& idx, std::string ch);
			void Remove(uint32 idx, bool _kill = false);
			void Remove(std::string ch, bool _kill = false);
			void Kill(std::string ch);
			Slyvina::VecString CharList();
		};

		// NEVER use directly. This is only a quick access class.
		class __IdData {
		private:
			_Char* Parent;
		public:
			inline __IdData() {}
			inline __IdData(_Char* P) { Parent = P; }
			std::string& operator[](std::string key);
		};

		class __IdBStat {
		private:
			_Char* Parent;
		public:
			inline __IdBStat() {}
			inline __IdBStat(_Char* P) { Parent = P; }
			Slyvina::int64& operator[](std::string key);
		};

		class _Char {
		private:
			static uint64 _cnt;
			uint64 _id{ _cnt++ };
			_Party* Parent{ nullptr };
			std::map<std::string, Stat> _Stats;
			int64 minimum{ 0 };
			int64 maximum{ 0 };
			bool usemini{ false };
			bool usemaxi{ false };
			std::map<std::string, Point> _Points;
			std::map<std::string, ChData> _DataMap;
			std::map<std::string, List> _Lijsten;
		public:
			_Char(_Party* P) { Parent = P; }
			inline uint64 ID() { return _id; }
			std::string Name;
			// Stats
			inline int64 Mini() { return minimum; }
			inline int64 Maxi() { return maximum; }
			inline void Mini(uint64 v) { minimum = v; maximum = std::max(minimum, maximum); usemini = true; }
			inline void Maxi(uint64 v) { maximum = v; minimum = std::min(minimum, maximum); usemaxi = true; }
			inline void KillMinMax() { minimum = 0; maximum = 0; usemini = false; usemaxi = false; }
			bool HaveStat(std::string sid);
			Stat Statistic(std::string sid);
			int64 Stats(std::string sid);
			int64 Stats(std::string sid, std::string modifier);
			void Stats(std::string sid, int64 v);
			void Stats(std::string sid, std::string modifier, int64 v);
			void LinkStat(std::string sourcestat, std::string targetchar, std::string targetstat);
			void LinkStat(std::string sourcestat, std::string targetchar);
			void KillStat(std::string stat);
			__IdBStat BStat{ this };
			Slyvina::Int64 TStat(std::string key);
			Slyvina::VecString StatList();

			// Points
			Point GetPoints(std::string sid);
			bool HavePoints(std::string sid);
			void Points(std::string sid, int32 v);
			void Points(std::string sid, int32 v, int32 max);

			void LinkPoints(std::string sourcestat, std::string targetchar, std::string targetstat);
			void LinkPoints(std::string sourcestat, std::string targetchar);
			VecString PointsList();

			// Data
			__IdData Data{ this };
			ChData GetData(std::string key);

			void LinkData(std::string sourcestat, std::string targetchar, std::string targetstat);
			void LinkData(std::string sourcestat, std::string targetchar);
			VecString DataList();

			// List
			List GetList(std::string key);
			void ListAdd(std::string key, std::string item);
			std::string& ListItem(std::string key, size_t idx);
			void LinkList(std::string sourcestat, std::string targetchar, std::string targetstat);
			void LinkList(std::string sourcestat, std::string targetchar);
			VecString ListList();
			size_t ListSize(std::string key);
		

		};


		class _Stat {
		private:
			static uint64 _cnt;
			uint64 _id{ _cnt++ };
			_Char* Parent;
			std::map<std::string, int64> Modifiers{};
			int64 minimum{ 0 };
			int64 maximum{ 0 };
			bool usemini{ false };
			bool usemaxi{ false };
		public:
			inline uint64 ID() { return _id; }
			inline _Char* GetCharacter() { return Parent; }
			StatScript StatScriptFunction;
			std::string StatScriptScript;
			int64 Base{ 0 };
			int64& operator[](std::string modid);
			int64 Total();
			inline int64 Mini() { return minimum; }
			inline int64 Maxi() { return maximum; }
			inline void Mini(int64 v) { minimum = v; maximum = std::max(minimum, maximum); usemini = true; }
			inline void Maxi(int64 v) { maximum = v; minimum = std::min(minimum, maximum); usemaxi = true; }
			inline bool UseMini() { return usemini; }
			inline bool UseMaxi() { return usemini; }
			inline void KillMinMax() { minimum = 0; maximum = 0; usemini = false; usemaxi = false; }
			VecString ListModifiers();
			_Stat(_Char* Ouwe);
		};

		class _Point {
		private:
			static uint64 _cnt;
			uint64 _id{ _cnt++ };
			_Char* Parent;
			int64
				_mini{ 0 },
				_maxi{ 0 },
				_have{ 0 };
		public:
			inline uint64 ID() { return _id; }
			_Point(_Char* Ouwe) {Parent = Ouwe; }
			std::string MaxCopy{""};
			std::string MinCopy{""};
			void Max(int64 m);
			void Min(int64 m);
			void Have(int64 v);
			int64 Max();
			int64 Min();
			int64 Have();
			inline void Inc(int64 v = 1) { Have(Have() + v); }
			inline void Dec(int64 v = 1) { Have(Have() - v); }
			inline void operator+=(int64 v) { Inc(v); }
			inline void operator-=(int64 v) { Dec(v); }
			inline bool operator==(const int64 v) { return Have() == v; }
			inline bool operator>(const int64 v) { return Have() > v; }
			inline bool operator<(const int64 v) { return Have() < v; }
			inline bool operator>=(const int64 v) { return Have() >= v; }
			inline bool operator<=(const int64 v) { return Have() <= v; }
			inline void operator=(const int64 v) { Have(v); }
			inline int64 operator+(const int64 v) { return Have() + v; }
			inline int64 operator-(const int64 v) { return Have() - v; }
		};


		class _Data {
		private:
			static uint64 _cnt;
			uint64 _id{ _cnt++ };
			_Char* Parent;
		public:
			inline uint64 ID() { return _id; }
			inline _Data(_Char* Ouwe) { Parent = Ouwe; }
			std::string Value{""};
		};


		class _List {
		private:
			static uint64 _cnt;
			uint64 _id{ _cnt++ };
			_Char* Parent;
			std::vector<std::string> _Lijst;
		public:
			inline uint64 ID() { return _id; }
			inline _List(_Char* Ouwe) { Parent = Ouwe; }
			inline void operator+=(std::string v) { _Lijst.push_back(v); }
			inline void sort() { SortVector(&_Lijst); }
			inline std::vector<std::string>* GetList() { return &_Lijst; }
			std::string& Spot(size_t i);
			inline std::string& operator[](size_t i) { return Spot(i); }
			void Remove(std::string item, bool all = false);
			void Remove(size_t idx);
			inline void operator-=(std::string v) { Remove(v); }
			inline void operator-=(size_t idx) { Remove(idx); }
		};

		inline Party CreateParty() { return std::shared_ptr<_Party>(new _Party()); }
		inline UParty CreateUniqueParty() { return std::unique_ptr<_Party>(new _Party()); }
	}
}