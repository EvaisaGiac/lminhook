#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
}

#ifdef _WIN32
#    define EXTERN extern "C" __declspec(dllexport)
#else
#    define EXTERN extern "C"
#endif


#define LUA_LOADED_TABLE "_LOADED"

#define lua_writestring(s,l)   fwrite((s), sizeof(char), (l), stdout)

#define lua_writeline()        (lua_writestring("\n", 1), fflush(stdout))

#define lua_writestringerror(s,p) \
        (fprintf(stderr, (s), (p)), fflush(stderr))

bool lua_isinteger(lua_State* L, int index);

#define LUA_OK		0
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3

void luaL_setfuncs(lua_State* L, const luaL_Reg* l, int nup);

#define luaL_newlib(L, R) do { lua_newtable(L); luaL_register(L, NULL, R); } while(0)

void luaL_requiref(lua_State* L, const char* modname, lua_CFunction openf, int glb);

const char* luaL_tolstring(lua_State* L, int idx, size_t* len);

int luaL_getsubtable(lua_State* L, int idx, const char* fname);

#define luaL_setfuncs socket_setfuncs
#define luaL_testudata socket_testudata
void luaL_setfuncs(lua_State* L, const luaL_Reg* l, int nup);
void* luaL_testudata(lua_State* L, int arg, const char* tname);

#define lua_getuservalue(L,idx)	lua_getiuservalue(L,idx,1)
#define lua_setuservalue(L,idx)	lua_setiuservalue(L,idx,1)

#define lua_rawlen(L,i) lua_objlen((L),(i))

void* lua_newuserdatauv(lua_State* L, size_t sz, int nuvalue);
int lua_getiuservalue(lua_State* L, int idx, int n);
int lua_setiuservalue(lua_State* L, int idx, int n);

int (lua_geti)(lua_State* L, int idx, lua_Integer n);

void lua_rawsetp(lua_State* L, int i, const void* p);

void lua_rawgetp(lua_State* L, int idx, const void* p);

void luaL_setmetatable(lua_State* L, const char* tname);

void lua_seti(lua_State* L, int index, lua_Integer n);

#define luaL_newlibtable(L,l) lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1);