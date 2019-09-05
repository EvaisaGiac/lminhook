#include "kmpFind.h"
#include <Windows.h>
#include <vector>
using namespace std;
#pragma warning( disable:4996 )

namespace kmpFind {
    class PatnNode {
    public:
        enum eMode { VAL, ANY };
        PatnNode(unsigned char val, eMode m) :val(val), mode(m) {};

        bool isAny() const { return mode == ANY; }

        eMode			mode;
        unsigned char	val;
    };

    vector<int> kmp_init(const vector<PatnNode> &p) {
        size_t len = p.size();

        vector<int> next(len + 1, 0);
        //for (size_t i = 1; i < len; i++) {
        //    int idx = next[i - 1];
        //    while (!p[i].isAny() && !p[idx].isAny() && idx > 0 && p[idx].val != p[i].val) {
        //        idx = next[idx - 1];
        //    }
        //    if (p[i].isAny() || p[idx].isAny() || p[idx].val == p[i].val) {
        //        next[i] = next[idx] + 1;
        //    } else {
        //        next[i] = 0;
        //    }
        //}
        size_t i = 1, j = 0;
        while (i < len) {
            if (j == 0 || p[i].val == p[j].val) {
                next[++i] = ++j;
            }
            else {
                j = next[j];
            }
        }
        return next;
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

    size_t kmp_find(const unsigned char *text, size_t text_len, const vector<PatnNode> &patns, const vector<int> &next) {
        size_t patn_len = patns.size();

        for (size_t i = 0, j = 0; i < text_len; ) {
            const unsigned char *p = text + i;
            unsigned char c;
            const unsigned char *text_v = kmp_readByte(p, &c);
            if (text_v > p) { // 发生异常
                i = text_v - text;
                j = 0;
            }

            if (patns[j].isAny() || patns[j].val == c) {
                j++;
                if (j == patn_len) {
                    return i + 1 - patn_len;
                }
            } else if (j > 0) {
                j = next[j - 1];
            }

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

    int Find(const unsigned char *text, int text_len, const char *patn, bool hex_patn) {
        size_t patn_len = strlen(patn);
        vector<PatnNode> patns;
        if (hex_patn) {
            char *p = new char[patn_len + 1];
            memcpy(p, patn, patn_len);
            p[patn_len] = 0;
            char *token = strtok(p, " ");
            while (token != 0) {
                if (token[0] == '.') {
                    patns.push_back(PatnNode(0, PatnNode::ANY));
                } else {
                    patns.push_back(PatnNode(HexToI(token), PatnNode::VAL));
                }
                token = strtok(0, " ");
            }
            delete[] p;
        } else {
            for (const char *p = patn; *p; p++) {
                if (*p == '.') {
                    patns.push_back(PatnNode(0, PatnNode::ANY));
                } else {
                    patns.push_back(PatnNode(*p, PatnNode::VAL));
                }
            }
        }

        auto next = kmp_init(patns);
        int pos = kmp_find(text, text_len, patns, next);
        return pos;
    }
}