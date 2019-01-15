#include "lminhook.h"
DWORD WINAPI cb_stdcall_0(hook *h) {
	lua_rawgeti(h->L, LUA_REGISTRYINDEX, h->callbackRef);
	if (lua_pcall(h->L, 0, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
	}
	return ((DWORD(WINAPI*)())(h->pOriginal))();
}

DWORD WINAPI cb_stdcall_1(hook *h, DWORD a1) {
	lua_rawgeti(h->L, LUA_REGISTRYINDEX, h->callbackRef);
	lua_pushinteger(h->L, a1);
	if (lua_pcall(h->L, 1, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
	}
	return ((DWORD(WINAPI*)(DWORD))(h->pOriginal))(a1);
}

DWORD WINAPI cb_stdcall_2(hook *h, DWORD a1, DWORD a2) {
	lua_rawgeti(h->L, LUA_REGISTRYINDEX, h->callbackRef);
	lua_pushinteger(h->L, a1);
	lua_pushinteger(h->L, a2);
	if (lua_pcall(h->L, 2, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
	}
	return ((DWORD(WINAPI*)(DWORD, DWORD))(h->pOriginal))(a1, a2);
}

DWORD WINAPI cb_stdcall_3(hook *h, DWORD a1, DWORD a2, DWORD a3) {
	lua_rawgeti(h->L, LUA_REGISTRYINDEX, h->callbackRef);
	lua_pushinteger(h->L, a1);
	lua_pushinteger(h->L, a2);
	lua_pushinteger(h->L, a3);
	if (lua_pcall(h->L, 3, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
	}
	return ((DWORD(WINAPI*)(DWORD, DWORD, DWORD))(h->pOriginal))(a1, a2, a3);
}

DWORD WINAPI cb_stdcall_4(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4) {
	lua_rawgeti(h->L, LUA_REGISTRYINDEX, h->callbackRef);
	lua_pushinteger(h->L, a1);
	lua_pushinteger(h->L, a2);
	lua_pushinteger(h->L, a3);
	lua_pushinteger(h->L, a4);
	if (lua_pcall(h->L, 4, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
	}
	return ((DWORD(WINAPI*)(DWORD, DWORD, DWORD, DWORD))(h->pOriginal))(a1, a2, a3, a4);
}

DWORD WINAPI cb_stdcall_5(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5) {
	lua_rawgeti(h->L, LUA_REGISTRYINDEX, h->callbackRef);
	lua_pushinteger(h->L, a1);
	lua_pushinteger(h->L, a2);
	lua_pushinteger(h->L, a3);
	lua_pushinteger(h->L, a4);
	lua_pushinteger(h->L, a5);
	if (lua_pcall(h->L, 5, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
	}
	return ((DWORD(WINAPI*)(DWORD, DWORD, DWORD, DWORD, DWORD))(h->pOriginal))(a1, a2, a3, a4, a5);
}

PVOID g_cb_stdcalls[] = {
	MakeStdCallDetour(cb_stdcall_0),
	MakeStdCallDetour(cb_stdcall_1),
	MakeStdCallDetour(cb_stdcall_2),
	MakeStdCallDetour(cb_stdcall_3),
	MakeStdCallDetour(cb_stdcall_4),
	MakeStdCallDetour(cb_stdcall_5),
};

LPVOID get_stdcall_cb(int nparams) {
	if (nparams >= 0 && nparams < sizeof(g_cb_stdcalls) / sizeof(g_cb_stdcalls[0])) {
		return g_cb_stdcalls[nparams];
	}
	return NULL;
}
