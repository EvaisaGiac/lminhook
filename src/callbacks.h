#pragma once

typedef DWORD(*CDECL_FUNC0)();
typedef DWORD(*CDECL_FUNC1)(DWORD);
typedef DWORD(*CDECL_FUNC2)(DWORD, DWORD);
typedef DWORD(*CDECL_FUNC3)(DWORD, DWORD, DWORD);
typedef DWORD(*CDECL_FUNC4)(DWORD, DWORD, DWORD, DWORD);
typedef DWORD(*CDECL_FUNC5)(DWORD, DWORD, DWORD, DWORD, DWORD);

typedef DWORD(WINAPI *STDCALL_FUNC0)();
typedef DWORD(WINAPI *STDCALL_FUNC1)(DWORD);
typedef DWORD(WINAPI *STDCALL_FUNC2)(DWORD, DWORD);
typedef DWORD(WINAPI *STDCALL_FUNC3)(DWORD, DWORD, DWORD);
typedef DWORD(WINAPI *STDCALL_FUNC4)(DWORD, DWORD, DWORD, DWORD);
typedef DWORD(WINAPI *STDCALL_FUNC5)(DWORD, DWORD, DWORD, DWORD, DWORD);

struct ThisCallObj {};
typedef DWORD(ThisCallObj::*MEM_FUNC0)();
typedef DWORD(ThisCallObj::*MEM_FUNC1)(DWORD);
typedef DWORD(ThisCallObj::*MEM_FUNC2)(DWORD, DWORD);
typedef DWORD(ThisCallObj::*MEM_FUNC3)(DWORD, DWORD, DWORD);
typedef DWORD(ThisCallObj::*MEM_FUNC4)(DWORD, DWORD, DWORD, DWORD);
typedef DWORD(ThisCallObj::*MEM_FUNC5)(DWORD, DWORD, DWORD, DWORD, DWORD);
union ThisCallPtr {
    LPVOID val;
    MEM_FUNC0 f0;
    MEM_FUNC1 f1;
    MEM_FUNC2 f2;
    MEM_FUNC3 f3;
    MEM_FUNC4 f4;
    MEM_FUNC5 f5;
};

DWORD cdeclcall_original_fn(hook *h, lua_State *L);
DWORD stdcall_original_fn(hook *h, lua_State *L);
DWORD thiscall_original_fn(hook *h, lua_State *L);

LPVOID get_cdecl_cb(int nparams);
LPVOID get_stdcall_cb(int nparams);
LPVOID get_thiscall_cb(int nparams);
