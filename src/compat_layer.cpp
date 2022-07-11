extern "C"
{
#include <lua.h>
}
#include <stdio.h>
#include "compat_layer.h"

/*
lua_isinteger(lua_State* L, int index) {
	int32_t x = (int32_t)lua_tointeger(L, index);
	lua_Number n = lua_tonumber(L, index);
	return ((lua_Number)x == n);
}
*/

bool lua_isinteger(lua_State* L, int index) {
	lua_Number x = lua_tointeger(L, index);
	lua_Number n = lua_tonumber(L, index);
	return (x == n);
}


void luaL_setfuncs(lua_State* L, const luaL_Reg* l, int nup) {
	luaL_checkstack(L, nup + 1, "too many upvalues");
	for (; l->name != NULL; l++) {  /* fill the table with given functions */
		int i;
		lua_pushstring(L, l->name);
		for (i = 0; i < nup; i++)  /* copy upvalues to the top */
			lua_pushvalue(L, -(nup + 1));
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_settable(L, -(nup + 3));
	}
	lua_pop(L, nup);  /* remove upvalues */
}

const char* luaL_tolstring(lua_State* L, int idx, size_t* len)
{
	if (!luaL_callmeta(L, idx, "__tostring")) {
		int t = lua_type(L, idx), tt = 0;
		char const* name = NULL;
		switch (t) {
		case LUA_TNIL:
			lua_pushliteral(L, "nil");
			break;
		case LUA_TSTRING:
		case LUA_TNUMBER:
			lua_pushvalue(L, idx);
			break;
		case LUA_TBOOLEAN:
			if (lua_toboolean(L, idx))
				lua_pushliteral(L, "true");
			else
				lua_pushliteral(L, "false");
			break;
		default:
			tt = luaL_getmetafield(L, idx, "__name");
			name = (tt == LUA_TSTRING) ? lua_tostring(L, -1) : lua_typename(L, t);
			lua_pushfstring(L, "%s: %p", name, lua_topointer(L, idx));
			if (tt != LUA_TNIL)
				lua_replace(L, -2);
			break;
		}
	}
	else {
		if (!lua_isstring(L, -1))
			luaL_error(L, "'__tostring' must return a string");
	}
	return lua_tolstring(L, -1, len);
}


int lua_absindex(lua_State* L, int i) {
	if (i < 0 && i > LUA_REGISTRYINDEX)
		i += lua_gettop(L) + 1;
	return i;
}

int luaL_getsubtable(lua_State* L, int idx, const char* fname)
{
	lua_getfield(L, idx, fname);
	if (lua_istable(L, -1))
		return 1;  /* table already there */
	else
	{
		lua_pop(L, 1);  /* remove previous result */
		idx = lua_absindex(L, idx);
		lua_newtable(L);
		lua_pushvalue(L, -1);  /* copy to be left at top */
		lua_setfield(L, idx, fname);  /* assign new table to field */
		return 0;  /* false, because did not find table there */
	}
}

void luaL_requiref(lua_State* L, const char* modname, lua_CFunction openf, int glb) {
	luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
	lua_getfield(L, -1, modname);  /* LOADED[modname] */
	if (!lua_toboolean(L, -1)) {  /* package not already loaded? */
		lua_pop(L, 1);  /* remove field */
		lua_pushcfunction(L, openf);
		lua_pushstring(L, modname);  /* argument to open function */
		lua_call(L, 1, 1);  /* call 'openf' to open module */
		lua_pushvalue(L, -1);  /* make copy of module (call result) */
		lua_setfield(L, -3, modname);  /* LOADED[modname] = module */
	}
	lua_remove(L, -2);  /* remove LOADED table */
	if (glb) {
		lua_pushvalue(L, -1);  /* copy of module */
		lua_setglobal(L, modname);  /* _G[modname] = module */
	}
}

void* luaL_testudata(lua_State* L, int ud, const char* tname) {
	void* p = lua_touserdata(L, ud);
	if (p != NULL) {  /* value is a userdata? */
		if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
			luaL_getmetatable(L, tname);  /* get correct metatable */
			if (!lua_rawequal(L, -1, -2))  /* not the same? */
				p = NULL;  /* value is a userdata with wrong metatable */
			lua_pop(L, 2);  /* remove both metatables */
			return p;
		}
	}
	return NULL;  /* value is not a userdata with a metatable */
}

void* lua_newuserdatauv(lua_State* L, size_t sz, int nuvalue)
{
	return lua_newuserdata(L, sz);
}

int lua_getiuservalue(lua_State* L, int idx, int n)
{
	if (n > 1)
	{
		lua_pushnil(L);
		return LUA_TNONE;
	}
	lua_getuservalue(L, idx);

	/* default environment is not a nil (see lua_getfenv) */
	lua_getglobal(L, "package");
	if (lua_rawequal(L, -2, -1) || lua_rawequal(L, -2, LUA_GLOBALSINDEX))
	{
		lua_pop(L, 2);
		lua_pushnil(L);

		return LUA_TNONE;
	}
	lua_pop(L, 1);	/* remove package */


	return lua_type(L, -1);
}

int lua_setiuservalue(lua_State* L, int idx, int n)
{
	if (n > 1

		|| lua_type(L, -1) != LUA_TTABLE

		)
	{
		lua_pop(L, 1);
		return 0;
	}

	(void)lua_setuservalue(L, idx);
	return 1; // I guess anything non-0 is ok
}


int lua_geti(lua_State* L, int index, lua_Integer i) {
	index = lua_absindex(L, index);
	lua_pushinteger(L, i);
	lua_gettable(L, index);
	return lua_type(L, -1);
}

void lua_rawsetp(lua_State* L, int i, const void* p) {
	int abs_i = lua_absindex(L, i);
	luaL_checkstack(L, 1, "not enough stack slots");
	lua_pushlightuserdata(L, (void*)p);
	lua_insert(L, -2);
	lua_rawset(L, abs_i);
}

void lua_rawgetp(lua_State* L, int idx, const void* p) {
	if (idx < 0) {
		idx += lua_gettop(L) + 1;
	}
	lua_pushlightuserdata(L, (void*)p);
	lua_rawget(L, idx);
}

void luaL_setmetatable(lua_State* L, const char* tname) {
	luaL_getmetatable(L, tname);
	lua_setmetatable(L, -2);
}

void lua_seti(lua_State* L, int index, lua_Integer n) {
	int absidx = lua_absindex(L, index);
	lua_pushvalue(L, -1);
	lua_pushinteger(L, n);
	lua_replace(L, -3);
	lua_rawset(L, absidx);
}