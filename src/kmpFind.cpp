#include "kmpFind.h"
#include <Windows.h>
#include <vector>
using namespace std;
#pragma warning( disable:4996 )

namespace kmpFind {
	class PatnNode {
	public:
		enum eMode { VAL, DOT };
		PatnNode(unsigned char val, eMode m) :m_val(val), m_mode(m) {};
		bool operator==(unsigned char c)const {
			if (m_mode == DOT)
				return true;
			else
				return m_val == c;
		}
		bool operator!=(unsigned char c)const {
			return !operator==(c);
		}
		bool operator==(const PatnNode &r)const {
			if (m_mode == DOT || r.m_mode == DOT)
				return true;
			else
				return m_val == r.m_val;
		}
		bool operator!=(const PatnNode &r)const {
			return !operator==(r);
		}
	private:
		eMode			m_mode;
		unsigned char	m_val;
	};

	void kmp_init(const vector<PatnNode> &patns, int *next) {
		size_t len = patns.size();

		next[0] = 0;
		for (size_t i = 1, j = 0; i < len; ++i) {
			while (j > 0 && patns[j] != patns[i])
				j = next[j - 1];
			if (patns[j] == patns[i])
				j++;
			next[i] = j;
		}
	}
	const unsigned char *kmp_readByte(const unsigned char *p, unsigned char *ret) {
		__try {
			*ret = *p;
			return p;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			const void *pt = p;
			while (true) {
				MEMORY_BASIC_INFORMATION    mbi;
				VirtualQuery(pt, &mbi, sizeof(mbi));
				pt = (char*)mbi.BaseAddress + mbi.RegionSize;
				if (!IsBadReadPtr(pt, 1))
					break;
			}
			p = (unsigned char *)pt;
			*ret = *p;
			return p;
		}
	}

	size_t kmp_find(const unsigned char *text, size_t text_len, const vector<PatnNode> &patns, int *next) {
		size_t patn_len = patns.size();

		for (size_t i = 0, j = 0; i < text_len; ) {
			const unsigned char *p = text + i;
			unsigned char c;
			const unsigned char *text_v = kmp_readByte(p, &c);
			if (text_v > p) {//发生异常
				i = text_v - text;
				j = 0;
			}

			while (j > 0 && patns[j] != c)
				j = next[j - 1];

			if (patns[j] == c)
				j++;

			if (j == patn_len)
				return i + 1 - patn_len;
			++i;
		}

		return -1;
	}

	unsigned char HexToI(char *hex) {
		unsigned char ret = 0;
		size_t len = strlen(hex);
		for (size_t i = 0; i < len && i < 2; ++i) {
			unsigned char c = toupper(hex[i]) - '0';
			if (c > 9) c -= 7;
			ret = (ret << 4) + c;
		}
		return ret;
	}

	int Find(const unsigned char *text, int text_len, const char *patn) {
		size_t patn_len = strlen(patn);

		char *p = new char[patn_len + 1];
		memcpy(p, patn, patn_len);
		p[patn_len] = 0;
		vector<PatnNode> patns;
		char *token = strtok(p, " ");
		while (token != 0) {
			//暂时不支持通配符
// 			if( token[0]=='.' )
// 				patns.push_back( PatnNode(0,PatnNode::DOT) );
// 			else
			patns.push_back(PatnNode(HexToI(token), PatnNode::VAL));
			token = strtok(0, " ");
		}
		delete[] p;

		int *next = new int[patns.size()];
		kmp_init(patns, next);
		int pos = kmp_find(text, text_len, patns, next);
		delete[] next;
		return pos;
	}
}