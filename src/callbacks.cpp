#include "lminhook.h"
#include "callbacks.h"
#include <functional>

int traceback(lua_State *L) {
	const char *msg = lua_tostring(L, 1);
	if (msg)
		luaL_traceback(L, L, msg, 1);
	else {
		lua_pushliteral(L, "(no error message)");
	}
	return 1;
}

DWORD WINAPI cb_0(hook *h) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		if (lua_pcall(h->L, 1, 1, -3)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

DWORD WINAPI cb_1(hook *h, DWORD a1) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		lua_pushinteger(h->L, a1); // param1
		if (lua_pcall(h->L, 2, 1, -4)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

DWORD WINAPI cb_2(hook *h, DWORD a1, DWORD a2) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		lua_pushinteger(h->L, a1); // param1
		lua_pushinteger(h->L, a2); // param2
		if (lua_pcall(h->L, 3, 1, -5)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

DWORD WINAPI cb_3(hook *h, DWORD a1, DWORD a2, DWORD a3) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		lua_pushinteger(h->L, a1); // param1
		lua_pushinteger(h->L, a2); // param2
		lua_pushinteger(h->L, a3); // param3
		if (lua_pcall(h->L, 4, 1, -6)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

DWORD WINAPI cb_4(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		lua_pushinteger(h->L, a1); // param1
		lua_pushinteger(h->L, a2); // param2
		lua_pushinteger(h->L, a3); // param3
		lua_pushinteger(h->L, a4); // param4
		if (lua_pcall(h->L, 5, 1, -7)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

DWORD WINAPI cb_5(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		lua_pushinteger(h->L, a1); // param1
		lua_pushinteger(h->L, a2); // param2
		lua_pushinteger(h->L, a3); // param3
		lua_pushinteger(h->L, a4); // param4
		lua_pushinteger(h->L, a5); // param5
		if (lua_pcall(h->L, 6, 1, -8)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

DWORD WINAPI cb_6(hook *h, DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6) {
	DWORD r = 0;
	lua_State *L = h->L;
	int top = lua_gettop(L);
	do {
		lua_getglobal(L, IDX_HOOK_REGISTER);

		lua_pushcfunction(L, traceback);

		lua_rawgeti(L, LUA_REGISTRYINDEX, h->callbackRef); // callback
		if (!lua_isfunction(L, -1)) break;

		lua_rawgetp(L, -3, h);
		if (!lua_isuserdata(L, -1)) break; // hook

		lua_pushinteger(h->L, a1); // param1
		lua_pushinteger(h->L, a2); // param2
		lua_pushinteger(h->L, a3); // param3
		lua_pushinteger(h->L, a4); // param4
		lua_pushinteger(h->L, a5); // param5
		lua_pushinteger(h->L, a6); // param6
		if (lua_pcall(h->L, 7, 1, -9)) {
			fprintf(stderr, "%s\n", lua_tostring(h->L, -1));
			break;
		}
		r = (DWORD)lua_tointeger(h->L, -1);
	} while (false);
	lua_settop(L, top);
	return r;
}

//////////////////////////////////////////////////////////////////////////
// cdecl

PVOID g_cb_cdecls[] = {
	MakeCdeclDetour(cb_0),
	MakeCdeclDetour(cb_1),
	MakeCdeclDetour(cb_2),
	MakeCdeclDetour(cb_3),
	MakeCdeclDetour(cb_4),
	MakeCdeclDetour(cb_5),
};

#define CB_PARAM_1 (DWORD)lua_tointeger(L, 2)
#define CB_PARAM_2 CB_PARAM_1, (DWORD)lua_tointeger(L, 3)
#define CB_PARAM_3 CB_PARAM_2, (DWORD)lua_tointeger(L, 4)
#define CB_PARAM_4 CB_PARAM_3, (DWORD)lua_tointeger(L, 5)
#define CB_PARAM_5 CB_PARAM_4, (DWORD)lua_tointeger(L, 6)

DWORD cdeclcall_original_fn(hook *h) {
	lua_State *L = h->L;
	switch (h->nparams) {
	case 0:return ((CDECL_FUNC0)h->pOriginal)();
	case 1:return ((CDECL_FUNC1)h->pOriginal)(CB_PARAM_1);
	case 2:return ((CDECL_FUNC2)h->pOriginal)(CB_PARAM_2);
	case 3:return ((CDECL_FUNC3)h->pOriginal)(CB_PARAM_3);
	case 4:return ((CDECL_FUNC4)h->pOriginal)(CB_PARAM_4);
	case 5:return ((CDECL_FUNC5)h->pOriginal)(CB_PARAM_5);
	default: return 0;
	}
}

LPVOID get_cdecl_cb(int nparams) {
	if (nparams >= 0 && nparams < sizeof(g_cb_cdecls) / sizeof(g_cb_cdecls[0])) {
		return g_cb_cdecls[nparams];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// thiscall

PVOID g_cb_thiscalls[] = {
	MakeThisCallDetour(cb_1),
	MakeThisCallDetour(cb_2),
	MakeThisCallDetour(cb_3),
	MakeThisCallDetour(cb_4),
	MakeThisCallDetour(cb_5),
	MakeThisCallDetour(cb_6),
};
#define THIS_CB_PARAM_1 (DWORD)lua_tointeger(L, 3)
#define THIS_CB_PARAM_2 THIS_CB_PARAM_1, (DWORD)lua_tointeger(L, 4)
#define THIS_CB_PARAM_3 THIS_CB_PARAM_2, (DWORD)lua_tointeger(L, 5)
#define THIS_CB_PARAM_4 THIS_CB_PARAM_3, (DWORD)lua_tointeger(L, 6)
#define THIS_CB_PARAM_5 THIS_CB_PARAM_4, (DWORD)lua_tointeger(L, 7)
#define CALL_MEMBER_FN(object, ptrToMember)  ((object)->*(ptrToMember))
DWORD thiscall_original_fn(hook *h) {
	ThisCallPtr p;
	lua_State *L = h->L;
	ThisCallObj *ECX_ = (ThisCallObj *)lua_tointeger(L, 2);
	p.val = h->pOriginal;
	switch (h->nparams) {
	case 0: return CALL_MEMBER_FN(ECX_, p.f0)();
	case 1: return CALL_MEMBER_FN(ECX_, p.f1)(THIS_CB_PARAM_1);
	case 2: return CALL_MEMBER_FN(ECX_, p.f2)(THIS_CB_PARAM_2);
	case 3: return CALL_MEMBER_FN(ECX_, p.f3)(THIS_CB_PARAM_3);
	case 4: return CALL_MEMBER_FN(ECX_, p.f4)(THIS_CB_PARAM_4);
	case 5: return CALL_MEMBER_FN(ECX_, p.f5)(THIS_CB_PARAM_5);
	default: return 0;
	}
}

LPVOID get_thiscall_cb(int nparams) {
	if (nparams >= 0 && nparams < sizeof(g_cb_thiscalls) / sizeof(g_cb_thiscalls[0])) {
		return g_cb_thiscalls[nparams];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// stdcall

PVOID g_cb_stdcalls[] = {
	MakeStdCallDetour(cb_0),
	MakeStdCallDetour(cb_1),
	MakeStdCallDetour(cb_2),
	MakeStdCallDetour(cb_3),
	MakeStdCallDetour(cb_4),
	MakeStdCallDetour(cb_5),
};

DWORD stdcall_original_fn(hook *h) {
	lua_State *L = h->L;
	switch (h->nparams) {
	case 0:return ((STDCALL_FUNC0)h->pOriginal)();
	case 1:return ((STDCALL_FUNC1)h->pOriginal)(CB_PARAM_1);
	case 2:return ((STDCALL_FUNC2)h->pOriginal)(CB_PARAM_2);
	case 3:return ((STDCALL_FUNC3)h->pOriginal)(CB_PARAM_3);
	case 4:return ((STDCALL_FUNC4)h->pOriginal)(CB_PARAM_4);
	case 5:return ((STDCALL_FUNC5)h->pOriginal)(CB_PARAM_5);
	default: return 0;
	}
}

LPVOID get_stdcall_cb(int nparams) {
	if (nparams >= 0 && nparams < sizeof(g_cb_stdcalls) / sizeof(g_cb_stdcalls[0])) {
		return g_cb_stdcalls[nparams];
	}
	return NULL;
}
