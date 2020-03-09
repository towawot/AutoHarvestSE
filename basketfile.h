#pragma once

#include <unordered_set>
#include <string>
#include <vector>

#define MODNAME "AutoHarvestSE.esp"

class BasketFile
{
public:
	enum listnum
	{
		USERLIST = 0,
		EXCLUDELIST,
		MAX,
	};

	UInt32 GetSize(listnum list_number);
	bool IsinList(listnum list_number, TESForm* form);
	bool SaveFile(listnum list_number, const char* basketText);
	bool LoadFile(listnum list_number, const char* basketText);

	void SyncList(listnum list_number);

	static BasketFile* GetSingleton(void)
	{
		if (!s_pInstance)
			s_pInstance = new BasketFile();
		return s_pInstance;
	}
private:
	BGSListForm* formList[MAX];
	std::unordered_set<TESForm*> list[MAX];
	static BasketFile* s_pInstance;

	BasketFile(void);
};

using namespace std;
class stringEx : public string
{
public:
	stringEx() : string() {}
	stringEx(const char *cc) : string(cc) {}
	stringEx(const string &str) : string(str) {}
	stringEx(const stringEx &str) : string(str) {}
	stringEx& operator=(const char *cc) { string::operator=(cc); return *this; }
	stringEx& operator=(const string& str) { string::operator=(str); return *this; }
	
	stringEx Format(const char* fmt, ...)
	{
		va_list list; va_start(list, fmt);
		char table[5000]; {
			vsprintf_s(table, 5000, fmt, list);
			assign(table);
		}
		va_end(list);
		return *this;
	}

	stringEx& EraceWord(vector<string> *list)
	{
		for (int index = 0; index < list->size(); index++)
		{
			string str = list->at(index);
			unsigned int loc = this->find(str, 0 );
			if( loc != string::npos ) {
				this->erase(loc, str.size());
			}
		}
		return *this;
	}

	stringEx& MakeUpper() {
		for (int index = 0; index < size(); index++) {
			at(index) = toupper(at(index));
		}
		return *this;
	}
};
