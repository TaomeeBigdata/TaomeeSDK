#include <sstream>

#include "statlogger.h"

using namespace std;

vector<string> stat_split(const string& s, char delim)
{
	std::string item;
	vector<string> elems;
    istringstream iss(s);
    while (getline(iss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void stat_trim(std::string& s, const std::string& charlist)
{
	// 去除字符串头部符合条件的字符
	string::size_type cnt = 0;
	for (; cnt != s.size(); ++cnt) {
		if (charlist.find(s[cnt]) == string::npos) {
			break;
		}
	}
	if (cnt) {
		s.erase(0, cnt);
	}
	if (s.empty()) {
		return;
	}
	// 去除字符串头部符合条件的字符
	cnt = 0;
	string::size_type pos = s.size() - 1;
	while (pos > 0) {
		if (charlist.find(s[pos]) == string::npos) {
			break;
		}
		--pos;
		++cnt;
	}
	if (cnt) {
		s.erase(pos + 1, cnt);
	}
}

/*
utf8编码范围
	00000000 -- 0000007F: 	0xxxxxxx
	00000080 -- 000007FF: 	110xxxxx (0xC0) 10xxxxxx (0x80)
	00000800 -- 0000FFFF: 	1110xxxx (0xE0) 10xxxxxx 10xxxxxx
	00010000 -- 001FFFFF: 	11110xxx (0xF0) 10xxxxxx 10xxxxxx 10xxxxxx
*/
bool stat_is_utf8(const string& s)
{
	string::size_type i = 0;
	while (i != s.size()) {
		string::size_type pos = i;
		if ((s[i] & 0x80) == 0) {
			++i;
			continue;
		} else if ((s[i] & 0xE0) == 0xC0) {
			i += 2;
		} else if ((s[i] & 0xF0) == 0xE0) {
			i += 3;
		} else if ((s[i] & 0xF8) == 0xF0) {
			i += 4;
		} else {
			return false;
		}

		if (i > s.size()) {
			return false;
		}
		for (++pos; pos != i; ++pos) {
			if ((s[pos] & 0xC0) != 0x80) {
				return false;
			}
		}
	}
	
	return true;
}


string stat_itostr(int32_t para)
{
	stringstream ss;
	ss<<para;
	string s = ss.str();
	return s;
}

string stat_itostr(int64_t para)
{
	stringstream ss;
	ss<<para;
	string s = ss.str();
	return s;
}

string stat_itostr(uint32_t para)
{
	stringstream ss;
	ss<<para;
	string s = ss.str();
	return s;
}

string stat_itostr(uint64_t para)
{
	stringstream ss;
	ss<<para;
	string s = ss.str();
	return s;
}
