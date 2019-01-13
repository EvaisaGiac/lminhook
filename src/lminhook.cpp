#include <lua.hpp>
#include <MinHook.h>
#include "lminhook.h"
#include <assert.h>
LPVOID get_cdecl_cb(int nparams);
LPVOID get_stdcall_cb(int nparams);

const char *call_type_str(int calltype) {
	switch (calltype) {
	case cdecl_type: return "cdecl";
	case stdcall_type: return "stdcall";
	case thiscall_type: return "thiscall";
	default: return "unknow calltype";
	}
}

LPVOID get_cb(int nparams, int calltype) {
	switch (calltype) {
	case cdecl_type: return get_cdecl_cb(nparams);
	case stdcall_type: return get_stdcall_cb(nparams);
	}
	return NULL;
}

static int lhook(lua_State *L) {
	hook *h = (hook *)lua_touserdata(L, 1);
	if (h != NULL) {
		MH_EnableHook(h->pTarget);
	}
	return 0;
}

static int lunhook(lua_State *L) {
	hook *h = (hook *)lua_touserdata(L, 1);
	if (h != NULL) {
		MH_DisableHook(h->pTarget);
	}
	return 0;
}

static int lgc(lua_State *L) {
	return lunhook(L);
}

static void minhook_meta(lua_State *L) {
	if (luaL_newmetatable(L, "minhook")) {
		luaL_Reg l[] = {
			{ "hook", lhook },
			{ "unhook", lunhook },
			{ NULL, NULL },
		};
		luaL_newlib(L, l);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, lgc);
		lua_setfield(L, -2, "__gc");
	}
	lua_setmetatable(L, -2);
}

static int linitialize(lua_State *L) {
	MH_STATUS s = MH_Initialize();
	lua_pushinteger(L, s);
	return 1;
}

static int luninitialize(lua_State *L) {
	MH_STATUS s = MH_Uninitialize();
	lua_pushinteger(L, s);
	return 1;
}

static int lcreatehook(lua_State *L) {
	auto target = luaL_checkinteger(L, 1);
	auto nparams = luaL_checkinteger(L, 2);
	int calltype = cdecl_type;
	if (lua_isnumber(L, 3)) {
		calltype = lua_tointeger(L, 3);
		luaL_checktype(L, 4, LUA_TFUNCTION);
	} else {
		luaL_checktype(L, 3, LUA_TFUNCTION);
	}
	LPVOID pOriginal = NULL;
	LPVOID pDetour = get_cb(nparams, calltype);
	if (pDetour == NULL) {
		luaL_error(L, "invalid params count %d with call type %s", nparams, call_type_str(calltype));
	}
	hook *h = (hook*)lua_newuserdata(L, sizeof(hook));
	MH_STATUS s = MH_CreateHook((LPVOID)target, pDetour, &pOriginal, h);
	if (s == MH_OK) {
		h->nparams = nparams;
		h->calltype = calltype;
		h->pOriginal = pOriginal;
		h->pTarget = (LPVOID)target;
		minhook_meta(L);
		return 1;
	}
	lua_pop(L, 1);
	return 0;
}
extern "C" LUAMOD_API int luaopen_minhook(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "initialize", linitialize },
		{ "uninitialize", luninitialize },
		{ "create", lcreatehook },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);
	return 1;
}
