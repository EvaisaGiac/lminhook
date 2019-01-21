#include <MinHook.h>
#include "lminhook.h"
#include <assert.h>
#include <string>
#include "kmpFind.h"
#include "callbacks.h"

const char *call_type_str(int calltype) {
	switch (calltype) {
	case cdecl_type: return "CDECL";
	case stdcall_type: return "STDCALL";
	case thiscall_type: return "THISCALL";
	default: return "UNKNOW";
	}
}

LPVOID get_cb(int nparams, int calltype) {
	switch (calltype) {
	case cdecl_type: return get_cdecl_cb(nparams);
	case stdcall_type: return get_stdcall_cb(nparams);
	case thiscall_type: return get_thiscall_cb(nparams);
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
		luaL_unref(L, LUA_REGISTRYINDEX, h->callbackRef);
	}
	return 0;
}

static int lcall_original(lua_State *L) {
	hook *h = (hook *)lua_touserdata(L, 1);
	if (h != NULL) {
		DWORD result = 0;
		if (h->calltype == cdecl_type) {
			result = cdeclcall_original_fn(h);
		} else if (h->calltype == stdcall_type) {
			result = stdcall_original_fn(h);
		} else if (h->calltype == thiscall_type) {
			result = thiscall_original_fn(h);
		}
		lua_pushinteger(L, (lua_Integer)result);
		return 1;
	}
	return 0;
}

static int lgc(lua_State *L) {
	return lunhook(L);
}

static int lcalltype(lua_State *L) {
	hook *h = (hook *)lua_touserdata(L, 1);
	if (h != NULL) {
		lua_pushstring(L, call_type_str(h->calltype));
		return 1;
	}
	return 0;
}

static int lsetcallback(lua_State *L) {
	hook *h = (hook *)lua_touserdata(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	if (h != NULL) {
		luaL_unref(L, LUA_REGISTRYINDEX, h->callbackRef);
		lua_pushvalue(L, 2);
		h->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	return 0;
}

static void minhook_meta(lua_State *L) {
	if (luaL_newmetatable(L, "minhook")) {
		luaL_Reg l[] = {
			{ "hook", lhook },
			{ "unhook", lunhook },
			{ "calltype", lcalltype },
			{ "setcallback", lsetcallback },
			{ NULL, NULL },
		};
		luaL_newlib(L, l);
		lua_setfield(L, -2, "__index");

		lua_pushcfunction(L, lgc);
		lua_setfield(L, -2, "__gc");

		lua_pushcfunction(L, lcall_original);
		lua_setfield(L, -2, "__call");
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

static void register_hook(hook *h) {
	lua_State *L = h->L;
	lua_getglobal(L, IDX_HOOK_REGISTER);
	lua_pushvalue(L, -2);
	lua_rawsetp(L, -2, h);
	lua_pop(L, 1);
}

static int lcreatehook(lua_State *L) {
	if (!lua_isinteger(L, 1) && !lua_isstring(L, 1)) {
		luaL_error(L, "invalid hook address");
	}
	int nparams = (int)luaL_checkinteger(L, 2);
	int calltype = cdecl_type;
	if (lua_isnumber(L, 3)) {
		calltype = (int)lua_tointeger(L, 3);
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
	MH_STATUS s;
	LPVOID target;
	if (lua_isinteger(L, 1)) {
		target = (LPVOID)luaL_checkinteger(L, 1);
		s = MH_CreateHook((LPVOID)target, pDetour, &pOriginal, h);
	} else {
		size_t len = 0;
		const char *p = lua_tolstring(L, 1, &len);
		const char *pp = strchr(p, ':');
		if (pp == NULL) {
			luaL_error(L, "invalid hook address");
		}
		std::string moduleName(p, pp - p), funcName(pp + 1);
		s = MH_CreateHookApiEx(moduleName.c_str(), funcName.c_str(), pDetour, &pOriginal, &target, h);
	}
	if (s == MH_OK) {
		h->nparams = nparams;
		h->calltype = calltype;
		h->pOriginal = pOriginal;
		h->pTarget = (LPVOID)target;
		h->L = L;
		lua_pushvalue(L, lua_isnumber(L, 3) ? 4 : 3);
		h->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
		minhook_meta(L);
		register_hook(h);
		return 1;
	}
	lua_pop(L, 1);
	return 0;
}

static int lkmdfind(lua_State *L) {
	int startAddr = (int)luaL_checkinteger(L, 1);
	int nCount = (int)luaL_checkinteger(L, 2);
	const char *patn = luaL_checkstring(L, 3);
	int pos = kmpFind::Find((const unsigned char *)startAddr, nCount, patn);
	if (pos >= 0) {
		lua_pushinteger(L, pos + startAddr);
		return 1;
	}
	return 0;
}

extern "C" LUAMOD_API int luaopen_minhook(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "initialize", linitialize },
		{ "uninitialize", luninitialize },
		{ "create", lcreatehook },
		{ "kmdfind", lkmdfind },
		{ NULL, NULL },
	};
	luaL_newlib(L, l);

	lua_pushinteger(L, cdecl_type);
	lua_setfield(L, -2, "CDECL");
	lua_pushinteger(L, stdcall_type);
	lua_setfield(L, -2, "STDCALL");
	lua_pushinteger(L, thiscall_type);
	lua_setfield(L, -2, "THISCALL");

	lua_newtable(L); // hook register weak table
	lua_newtable(L); // metatable={}
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3); // metatable._mode='v'
	lua_setmetatable(L, -2); // setmetatable(hook register table, metatable)
	lua_setglobal(L, IDX_HOOK_REGISTER);

	return 1;
}
