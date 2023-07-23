// Lic:
// Statistician/Statistician.cpp
// Statistician
// version: 23.07.23
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

#include <SlyvString.hpp>

#include "Statistician.hpp"

#define STAT_DEBUG

#ifdef STAT_DEBUG
#include <iostream>
#define Chat(ABC) std::cout << "\x1b[32mDEBUG>\x1b[0m; " << ABC <<"\n"
#define Warn(ABC) std::cout << "\x1b[31mWARNING>\x1b[0m; " << ABC <<"\n"
#else
#define Chat()
#endif

using namespace std;
using namespace Slyvina::Units;

namespace Slyvina {
	namespace Statistician {
		uint64 _Party::_cnt{0};
		uint64 _Char::_cnt{0};
		uint64 _Stat::_cnt{0};
		uint64 _Data::_cnt{0};
		uint64 _List::_cnt{0};
		uint64 _Point::_cnt{0};

		Slyvina::Statistician::_Party::_Party() {
			_id = _cnt++;
			Chat("Created Party Record #" << _id);
			Size(4);
		}
		_Party::~_Party() {
			_KillParty();
			Chat("Disposed Party Record #" << _id);
		}
		void _Party::Size(uint32 s) {
			if (!s) throw std::runtime_error("Party size must at least 1 or higher!");
			auto OSize{ _PSize };
			auto OParty{ _PartyArray };
			_PartyArray = new std::string[s];
			if (OSize && OParty) {
				for (uint32 i = 0; i < OSize && i < s; ++i) {
					(*_PartyArray)[i] = (*OParty)[i];
				}
				delete[] OParty;
			}
		}

		uint32 _Party::PartyMembers() {
			uint32 ret{ 0 };
			for (uint32 i = 0; i < _PSize; ++i) if (_PartyArray[i].size()) ret++;
			return ret;
		}

		Character _Party::Ch(uint32 idx) {
			if (idx >= _PSize) throw runtime_error(TrSPrintF("Party index out of bounds (%d/%d)", idx, _PSize));
			if (!_PartyArray[idx].size()) throw runtime_error(TrSPrintF("Party index %d is empty", idx));
			return Ch(_PartyArray[idx]);
		}

		Character _Party::Ch(std::string id) {
			Trans2Upper(id);
			if (!_Characters.count(id)) throw runtime_error("Character \"" + id + "\" does not exist");
			return _Characters[id];
		}
		Character _Party::NewChar(bool autoaddtoparty, std::string id, std::string name) {
			auto ret{ new _Char(this) };
			if (!name.size()) name = id;
			Trans2Upper(id);
			ret->Name = name;
#ifdef STAT_DEBUG
			if (_Characters.count(id)) Warn("Character \"" << id << "\" already exists! Overwriting!");
#endif
			auto sret{ std::shared_ptr<_Char>(ret) };
			_Characters[id] = sret;
			Chat("Character \"" << id << "\" created as #" << ret->ID());
			if (autoaddtoparty) AddToParty(id);
			return sret;
		}
		void _Party::AddToParty(std::string cid, bool ignorewhenfull) {
			for (uint32 i = 0; i < _PSize; ++i) {
				if (!_PartyArray[i].size()) {
					_PartyArray[i] = cid;
					return;
				}
			}
			Warn("Max party length exceeded");
			if (!ignorewhenfull) throw std::runtime_error("Max party length exceeded");
		}
		std::string _Party::PIndex(uint32 idx) {
			if (idx >= _PSize) throw runtime_error(TrSPrintF("Party index out of bounds (%d/%d)", idx, _PSize));
			return _PartyArray[idx];
		}

		void _Party::FindInParty(bool& success, uint32& idx, std::string ch) {
			Trans2Upper(ch);
			success = false;
			for (uint32 i = 0; i < _PSize; ++i) {
				if (_PartyArray[i] == ch) {
					success = true;
					idx = i;
					return;
				}
			}
		}

		void _Party::Remove(uint32 idx, bool _kill) {
			if (idx >= _PSize || _PartyArray[idx].size()) return;
			if (_kill) {
				auto k{ _PartyArray[idx] };
				Kill(k);
			}
			for (uint32 i = idx; i < _PSize - 1; ++i) _PartyArray[i] = _PartyArray[i + 1];
			_PartyArray[_PSize - 1] = "";
		}

		void _Party::Remove(std::string ch, bool _kill) {
			uint32 p;
			bool f;
			FindInParty(f, p, ch);
			if (f) Remove(p, _kill);
		}

		void _Party::Kill(std::string ch) {
			Trans2Upper(ch);
			if (!_Characters.count(ch)) return;
			Remove(ch, false); // 2nd MUST always be false or an infinite loop WILL form freezing the program!
			_Characters.erase(ch);
		}
		int64& _Stat::operator[](std::string modid) {
			Trans2Upper(modid);
			return Modifiers[modid];
		}
		int64 _Stat::Total() {
			if (StatScriptFunction) StatScriptFunction(this, StatScriptScript);
			auto ret{ Base };
			for (auto& k : Modifiers) {
				Base += k.second;
			}
			if (usemini) ret = std::max(minimum, ret);
			if (usemaxi) ret = std::min(maximum, ret);
		}

		_Stat::_Stat(_Char* Ouwe) {
			Parent = Ouwe;
		}
		bool _Char::HaveStat(std::string sid) {
			Trans2Upper(sid);
			return _Stats.count(sid);
		}
		Stat _Char::Statistic(std::string sid) {
			Trans2Upper(sid);
			if (!_Stats.count(sid)) {
				auto ret{ new _Stat(this) };
				if (usemaxi) ret->Maxi(maximum);
				if (usemini) ret->Mini(minimum);
				_Stats[sid] = std::shared_ptr<_Stat>(ret);
			}
			return _Stats[sid];
		}
		int64 _Char::Stats(std::string sid) { return Statistic(sid)->Total(); }
		int64 _Char::Stats(std::string sid, std::string modifier) {
			Trans2Upper(modifier);
			if (modifier == "BASE")  return Statistic(sid)->Base;
			return (*Statistic(sid))[modifier];
		}

		void _Char::Stats(std::string sid, int64 v) { Statistic(sid)->Base = v; }
		void _Char::Stats(std::string sid, std::string modifier, int64 v) {
			Trans2Upper(modifier);
			if (modifier == "BASE")  Statistic(sid)->Base = v;
			(*Statistic(sid))[modifier] = v;
		}
		void _Char::LinkStat(std::string sourcestat, std::string targetchar, std::string targetstat) {
			auto src{ this };
			auto tgt{ Parent->Ch(targetchar) };
			//Trans2Upper(sourcestat);
			Trans2Upper(targetstat);
			tgt->_Stats[targetstat] = Statistic(sourcestat);
		}
		void _Char::LinkStat(std::string sourcestat, std::string targetchar) { LinkStat(sourcestat, targetchar, sourcestat); }
		void _Char::KillStat(std::string stat) {
			Trans2Upper(stat);
			if (_Stats.count(stat)) _Stats.erase(stat);
		}
		Point _Char::GetPoints(std::string sid) {
			Trans2Upper(sid);
			if (!_Points.count(sid)) {
				auto ret = new _Point(this);
				_Points[sid] = std::shared_ptr<_Point>(ret);
			}
			return _Points[sid];
		}

		bool _Char::HavePoints(std::string sid) {
			Trans2Upper(sid);
			return _Points.count(sid);
		}

		void _Char::Points(std::string sid, int32 v) { GetPoints(sid)->Have(v); }

		void _Char::Points(std::string sid, int32 v, int32 max) {
			auto p{ GetPoints(sid) };
			*p = v;
			p->Max(max);
		}

		void _Char::LinkPoints(std::string sourcestat, std::string targetchar, std::string targetstat) {
			//Trans2Upper(sourcestat);
			Trans2Upper(targetstat);
			auto src{ this };
			auto tgt{ Parent->Ch(targetchar) };
			auto pnt{ GetPoints(sourcestat) };
			_Points[targetstat] = pnt;
			if (pnt->MaxCopy.size()) LinkStat(pnt->MaxCopy, targetchar);
			if (pnt->MinCopy.size()) LinkStat(pnt->MinCopy, targetchar);
		}

		void _Char::LinkPoints(std::string sourcestat, std::string targetchar) { LinkPoints(sourcestat, targetchar, sourcestat); }

		ChData _Char::GetData(std::string key) {
			Trans2Upper(key);
			if (!_DataMap.count(key)) {
				_DataMap[key] = std::shared_ptr<_Data>(new _Data(this));
			}
			return _DataMap[key];
		}

		int32 _Point::Max() {
			if (MaxCopy.size()) _maxi = Parent->Stats(MaxCopy);
			return _maxi;
		}
		int32 _Point::Min() {
			if (MinCopy.size()) _mini = Parent->Stats(MinCopy);
			return _mini;
		}

		void _Point::Have(int32 v) {
			_have = std::min(v, Max());
			_have = std::max(_have, Min());
		}
		int32 _Point::Have() {
			_have = std::min(_have, Max());
			_have = std::max(_have, Min());
			return _have;
		}
		void _Point::Max(int32 m) { _maxi = std::max(m, Min()); }
		void _Point::Min(int32 m) { _mini = m; _maxi = std::max(m, _maxi); }

		std::string& __IdData::operator[](std::string key) {
			return Parent->GetData(key)->Value;
		}

		void _Char::LinkData(std::string sourceData, std::string targetchar, std::string targetData) {
			auto src{ this };
			auto tgt{ Parent->Ch(targetchar) };
			//Trans2Upper(sourceData);
			Trans2Upper(targetData);
			tgt->_DataMap[targetData] = GetData(sourceData);
		}
		void _Char::LinkData(std::string sourceData, std::string targetchar) { LinkData(sourceData, targetchar, sourceData); }

		List _Char::GetList(std::string key) {
			Trans2Upper(key);
			if (!_Lijsten.count(key)) {
				_Lijsten[key] = shared_ptr<_List>(new _List(this));
			}
			return _Lijsten[key];
		}
		void _Char::ListAdd(std::string key, std::string item) { *GetList(key) += item; }

		std::string& _Char::ListItem(std::string key, size_t idx) { return (*GetList(key))[idx]; }

		std::string& _List::Spot(size_t i) {
			if (i >= _Lijst.size()) throw std::runtime_error(TrSPrintF("List index out of bounds (%d/%d)", i, _Lijst.size()));
			return _Lijst[i];
		}
		void _List::Remove(std::string item, bool all) {
			bool first{ true };
			vector<string> Temp{};
			for (auto _it : _Lijst) {
				if (!(_it == item && (all || first))) {
					first = false;
				} else Temp.push_back(_it);
			}
			_Lijst = Temp;
		}
		void _List::Remove(size_t idx) {
			vector<string> Temp{};
			for (size_t i = 0; i < _Lijst.size(); i++)
				if (i != idx) Temp.push_back(_Lijst[i]);
			_Lijst = Temp;
		}

		void _Char::LinkList(std::string sourceLijst, std::string targetchar, std::string targetLijst) {
			auto src{ this };
			auto tgt{ Parent->Ch(targetchar) };
			//Trans2Upper(sourceLijst);
			Trans2Upper(targetLijst);
			tgt->_Lijsten[targetLijst] = GetList(sourceLijst);
		}

		void _Char::LinkList(std::string sourceLijst, std::string targetchar) { LinkList(sourceLijst, targetchar, sourceLijst); }

	}
}