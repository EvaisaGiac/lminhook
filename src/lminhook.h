#pragma once
#include <windows.h>
#include <lua.hpp>

#define cdecl_type 0
#define stdcall_type 1
#define thiscall_type 3

struct hook {
	LPVOID pTarget;
	LPVOID pOriginal;
	int nparams;
	int calltype;
	int callbackRef;
	lua_State *L;
};

#pragma code_seg(".h")
typedef DWORD(WINAPI*FUNC0)(hook *);
typedef DWORD(WINAPI*FUNC1)(hook *, DWORD);
typedef DWORD(WINAPI*FUNC2)(hook *, DWORD, DWORD);
typedef DWORD(WINAPI*FUNC3)(hook *, DWORD, DWORD, DWORD);
typedef DWORD(WINAPI*FUNC4)(hook *, DWORD, DWORD, DWORD, DWORD);
typedef DWORD(WINAPI*FUNC5)(hook *, DWORD, DWORD, DWORD, DWORD, DWORD);

template< unsigned int unique_no >
static __declspec(naked) DWORD HookFuncWarp() {
	__asm {
		__emit 0E8h;
		__emit 0; __emit 0; __emit 0; __emit 0;
		__emit 0; __emit 0; __emit 0; __emit 0;
	}
}

#define SAVE_FP __asm {__asm push ebp __asm mov ebp, esp __asm pushad __asm pushfd}
#define CALL_0 __asm { \
	__asm push[ebp + 0Ch] \
	__asm mov eax, dword ptr[ebp + 4] \
	__asm call[eax] \
	__asm mov[ebp + 4], eax \
}
#define CALL_1 __asm {__asm push[ebp + 10h]} CALL_0
#define CALL_2 __asm {__asm push[ebp + 14h]} CALL_1
#define CALL_3 __asm {__asm push[ebp + 18h]} CALL_2
#define CALL_4 __asm {__asm push[ebp + 1Ch]} CALL_3
#define CALL_5 __asm {__asm push[ebp + 20h]} CALL_4
#define POP_FP __asm { \
	__asm popfd \
	__asm popad \
	__asm mov esp, ebp \
	__asm pop ebp \
	/* 在堆栈上交换钩子补充的hook *参数和返回地址 */ \
	__asm mov eax, dword ptr[esp + 4] \
	__asm mov dword ptr[esp + 8], eax \
	/* 弹出返回值到eax */ \
	__asm pop eax \
}
#define RETN(n) {__asm retn n}
#define RET {__asm ret}

//////////////////////////////////////////////////////////////////////////
// CdeclDetour
template<typename T>
static __declspec(naked) DWORD CdeclDetour();
template<> static __declspec(naked) 
DWORD CdeclDetour<FUNC0>() { SAVE_FP; CALL_0; POP_FP; RETN(4); }
template<> static __declspec(naked) 
DWORD CdeclDetour<FUNC1>() { SAVE_FP; CALL_1; POP_FP; RETN(4); }
template<> static __declspec(naked) 
DWORD CdeclDetour<FUNC2>() { SAVE_FP; CALL_2; POP_FP; RETN(4); }
template<> static __declspec(naked) 
DWORD CdeclDetour<FUNC3>() { SAVE_FP; CALL_3; POP_FP; RETN(4); }
template<> static __declspec(naked) 
DWORD CdeclDetour<FUNC4>() { SAVE_FP; CALL_4; POP_FP; RETN(4); }
template<> static __declspec(naked) 
DWORD CdeclDetour<FUNC5>() { SAVE_FP; CALL_5; POP_FP; RETN(4); }

//////////////////////////////////////////////////////////////////////////
// StdCallDetour
template<typename T>
static __declspec(naked) DWORD StdCallDetour();
template<> static __declspec(naked)
DWORD StdCallDetour<FUNC0>() { SAVE_FP; CALL_0; POP_FP; RETN(4); }
template<> static __declspec(naked)
DWORD StdCallDetour<FUNC1>() { SAVE_FP; CALL_1; POP_FP; RETN(8); }
template<> static __declspec(naked)
DWORD StdCallDetour<FUNC2>() { SAVE_FP; CALL_2; POP_FP; RETN(0x0C); }
template<> static __declspec(naked)
DWORD StdCallDetour<FUNC3>() { SAVE_FP; CALL_3; POP_FP; RETN(0x10); }
template<> static __declspec(naked)
DWORD StdCallDetour<FUNC4>() { SAVE_FP; CALL_4; POP_FP; RETN(0x14); }
template<> static __declspec(naked)
DWORD StdCallDetour<FUNC5>() { SAVE_FP; CALL_5; POP_FP; RETN(0x18); }

#pragma code_seg()
#pragma comment(linker,"/SECTION:.h,RWE")

template<unsigned int unique_no, typename T>
inline PROC MakeCdeclDetour_(T FUNC) {
	DWORD(*f1)() = &HookFuncWarp<unique_no>;
	DWORD(*f2)() = &CdeclDetour<T>;
	__asm {
		push FUNC;
		mov eax, f2;
		sub eax, f1;
		sub eax, 5;
		push eax;
		mov eax, f1;
		pop dword ptr[eax + 1];
		pop dword ptr[eax + 5];
	}
	return (PROC)f1;
}

template<unsigned int unique_no, typename T>
inline PROC MakeStdCallDetour_(T FUNC) {
	DWORD(*f1)() = &HookFuncWarp<unique_no>;
	DWORD(*f2)() = &StdCallDetour<T>;
	__asm {
		push FUNC;
		mov eax, f2;
		sub eax, f1;
		sub eax, 5;
		push eax;
		mov eax, f1;
		pop dword ptr[eax + 1];
		pop dword ptr[eax + 5];
	}
	return (PROC)f1;
}

#define MakeCdeclDetour(func) MakeCdeclDetour_<__COUNTER__>(func)
#define MakeStdCallDetour(func) MakeStdCallDetour_<__COUNTER__>(func)
