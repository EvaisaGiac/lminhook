#include "lminhook.h"

DWORD WINAPI cb_cdecl_0(hook *h) {
	return ((DWORD(*)())(h->pOriginal))();
}

DWORD WINAPI cb_cdecl_1(hook *h, DWORD a1) {
	return ((DWORD(*)(DWORD))(h->pOriginal))(a1);
}

DWORD WINAPI cb_cdecl_2(hook *h, DWORD a1, DWORD a2) {
	return ((DWORD(*)(DWORD, DWORD))(h->pOriginal))(a1, a2);
}

DWORD WINAPI cb_cdecl_3(hook *h, DWORD a1, DWORD a2, DWORD a3) {
	return ((DWORD(*)(DWORD, DWORD, DWORD))(h->pOriginal))(a1, a2, a3);
}

DWORD WINAPI cb_cdecl_4(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4) {
	return ((DWORD(*)(DWORD, DWORD, DWORD, DWORD))(h->pOriginal))(a1, a2, a3, a4);
}

DWORD WINAPI cb_cdecl_5(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5) {
	return ((DWORD(*)(DWORD, DWORD, DWORD, DWORD, DWORD))(h->pOriginal))(a1, a2, a3, a4, a5);
}

PVOID g_cb_cdecls[] = {
	MakeCdeclDetour(cb_cdecl_0),
	MakeCdeclDetour(cb_cdecl_1),
	MakeCdeclDetour(cb_cdecl_2),
	MakeCdeclDetour(cb_cdecl_3),
	MakeCdeclDetour(cb_cdecl_4),
	MakeCdeclDetour(cb_cdecl_5),
};

LPVOID get_cdecl_cb(int nparams) {
	if (nparams >= 0 && nparams < sizeof(g_cb_cdecls) / sizeof(g_cb_cdecls[0])) {
		return g_cb_cdecls[nparams];
	}
	return NULL;
}
