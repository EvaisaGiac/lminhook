#include <lua.hpp>
#include <assert.h>
#include <string>
#include <windows.h>
#define cdecl_type 0
#define stdcall_type 1
#define thiscall_type 3

int cdecl_test0() { return 0; }
int cdecl_test1(int a) { return a; }
int cdecl_test2(int a, int b) { return a + b; }
int cdecl_test3(int a, int b, int c) { return a + b + c; }
int cdecl_test4(int a, int b, int c, int d) { return a + b + c + d; }
int cdecl_test5(int a, int b, int c, int d, int e) { return a + b + c + d + e; }

int WINAPI stdcall_test0() { return 0; }
int WINAPI stdcall_test1(int a) { return a; }
int WINAPI stdcall_test2(int a, int b) { return a + b; }
int WINAPI stdcall_test3(int a, int b, int c) { return a + b + c; }
int WINAPI stdcall_test4(int a, int b, int c, int d) { return a + b + c + d; }
int WINAPI stdcall_test5(int a, int b, int c, int d, int e) { return a + b + c + d + e; }

const char *test_code = " \
	local t = require(\"minhook\") \
	t.initialize() \
	local test_hooks = {} \
	local f = function(ct, ...) return ct, {...} end \
	local calltype, args = f(...) \
	for i, v in ipairs(args) do \
		local h = t.create(v, i - 1, calltype, function() end) \
		table.insert(test_hooks, h) \
		h:hook() \
	end \
";

void cdecl_tests() {
	lua_State *l = luaL_newstate();
	luaL_openlibs(l);
	int err = luaL_loadbufferx(l, test_code, strlen(test_code), "=[]", "t");
	assert(err == LUA_OK);
	lua_pushinteger(l, cdecl_type);
	lua_pushinteger(l, (lua_Integer)&cdecl_test0);
	lua_pushinteger(l, (lua_Integer)&cdecl_test1);
	lua_pushinteger(l, (lua_Integer)&cdecl_test2);
	lua_pushinteger(l, (lua_Integer)&cdecl_test3);
	lua_pushinteger(l, (lua_Integer)&cdecl_test4);
	lua_pushinteger(l, (lua_Integer)&cdecl_test5);
	err = lua_pcall(l, 7, 0, 0);
	if (err) {
		fprintf(stderr, "%s\n", lua_tostring(l, -1));
		lua_close(l);
		return;
	}
	printf("test0: %d\n", cdecl_test0());
	printf("test1: %d\n", cdecl_test1(1));
	printf("test2: %d\n", cdecl_test2(1, 1));
	printf("test3: %d\n", cdecl_test3(1, 1, 1));
	printf("test4: %d\n", cdecl_test4(1, 1, 1, 1));
	printf("test5: %d\n", cdecl_test5(1, 1, 1, 1, 1));
	lua_close(l);
}

void stdcall_tests() {
	lua_State *l = luaL_newstate();
	luaL_openlibs(l);
	int err = luaL_loadbufferx(l, test_code, strlen(test_code), "=[]", "t");
	assert(err == LUA_OK);
	lua_pushinteger(l, stdcall_type);
	lua_pushinteger(l, (lua_Integer)&stdcall_test0);
	lua_pushinteger(l, (lua_Integer)&stdcall_test1);
	lua_pushinteger(l, (lua_Integer)&stdcall_test2);
	lua_pushinteger(l, (lua_Integer)&stdcall_test3);
	lua_pushinteger(l, (lua_Integer)&stdcall_test4);
	lua_pushinteger(l, (lua_Integer)&stdcall_test5);
	err = lua_pcall(l, 7, 0, 0);
	if (err) {
		fprintf(stderr, "%s\n", lua_tostring(l, -1));
		lua_close(l);
		return;
	}
	printf("stdcall_test0: %d\n", stdcall_test0());
	printf("stdcall_test1: %d\n", stdcall_test1(1));
	printf("stdcall_test2: %d\n", stdcall_test2(1, 1));
	printf("stdcall_test3: %d\n", stdcall_test3(1, 1, 1));
	printf("stdcall_test4: %d\n", stdcall_test4(1, 1, 1, 1));
	printf("stdcall_test5: %d\n", stdcall_test5(1, 1, 1, 1, 1));
	lua_close(l);
}

int main() {
	cdecl_tests();
	stdcall_tests();
	system("pause");
	return 0;
}
