#include <MinHook.h>
#include "lminhook.h"
#include <assert.h>
#include <string>
#include "kmpFind.h"
#include "callbacks.h"
#include "compat_layer.h"

void print(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    char tmp[4096];
    vsnprintf_s(tmp, sizeof(tmp), fmt, va);
    va_end(va);
    OutputDebugStringA(tmp);
}

const char *call_type_str(int calltype) {
    switch (calltype) {
    case cdecl_type: return "CDECL";
    case stdcall_type: return "STDCALL";
    case thiscall_type: return "THISCALL";
    case vtblhook_type: return "VTABLE";
    default: return "UNKNOW";
    }
}

LPVOID get_cb(int nparams, int calltype) {
    switch (calltype) {
    case cdecl_type: return get_cdecl_cb(nparams);
    case stdcall_type: return get_stdcall_cb(nparams);
    case thiscall_type: return get_thiscall_cb(nparams);
    case vtblhook_type: return get_thiscall_cb(nparams);
    }
    return NULL;
}

int lsetcallback(lua_State *L);
static int lhook(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (lua_gettop(L) > 1) {
        lsetcallback(L);
    }
    if (h != NULL) {
        MH_EnableHook(h->pTarget);
    }
    return 0;
}

static int lunhook(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (h != NULL) {
        MH_DisableHook(h->pTarget);
        if (h->callbackRef != -1) {
            luaL_unref(L, LUA_REGISTRYINDEX, h->callbackRef);
        }
    }
    return 0;
}

static int _call_original(lua_State *L, hook *h) {
    if (h != NULL) {
        DWORD result = 0;
        if (h->calltype == cdecl_type) {
            result = cdeclcall_original_fn(h, L);
        } else if (h->calltype == stdcall_type) {
            result = stdcall_original_fn(h, L);
        } else if (h->calltype == thiscall_type || h->calltype == vtblhook_type) {
            result = thiscall_original_fn(h, L);
        }
        lua_pushinteger(L, (lua_Integer)result);
    } else {
        lua_pushinteger(L, 0);
    }
    return 1;
}

static int lcall_original(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    return _call_original(L, h);
}

static int lgc(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (h != NULL) {
        MH_RemoveHook(h->pTarget);
        if (h->callbackRef != -1) {
            luaL_unref(L, LUA_REGISTRYINDEX, h->callbackRef);
        }
    }
    return 0;
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
    if (h != NULL) {
        if (h->callbackRef != -1) {
            luaL_unref(L, LUA_REGISTRYINDEX, h->callbackRef);
            h->callbackRef = -1;
        }
        if (!lua_isnil(L, 2)) {
            luaL_checktype(L, 2, LUA_TFUNCTION);
            lua_pushvalue(L, 2);
            h->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
        }
    }
    return 0;
}

static int lecx(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (h != NULL) {
        lua_pushinteger(L, h->Ecx);
        return 1;
    }
    return 0;
}

static int lsetresult(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (h != NULL) {
        if (lua_gettop(L) < 2 || lua_isnil(L, 2)) {
            h->result = 0;
            h->hasResult = false;
        } else {
            h->result = (int)luaL_checkinteger(L, 2);
            h->hasResult = true;
        }
    }
    return 0;
}

static int lgetresult(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (h != NULL && h->hasResult) {
        lua_pushinteger(L, h->result);
        return 1;
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
            { "ecx", lecx },
            { "setresult", lsetresult },
            { "getresult", lgetresult },
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

static int lshowconsole(lua_State *L) {
    // allocate a console for this app
    if (AllocConsole()) {
        AttachConsole(GetCurrentProcessId());
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
        freopen("CON", "r", stdin);
    }
    return 0;
}

static int lhideconsole(lua_State *L) {
    fclose(stdout);
    fclose(stderr);
    fclose(stdin);
    FreeConsole();
    return 0;
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
    bool hasCallbackFunc = false;
    if (lua_isnumber(L, 3)) {
        calltype = (int)lua_tointeger(L, 3);
        if (lua_gettop(L) == 4 && !lua_isnil(L, 4)) {
            luaL_checktype(L, 4, LUA_TFUNCTION);
            hasCallbackFunc = true;
        }
    } else if (!lua_isnil(L, 3)) {
        luaL_checktype(L, 3, LUA_TFUNCTION);
        hasCallbackFunc = true;
    }
    LPVOID pOriginal = NULL;
    LPVOID pDetour = get_cb(nparams, calltype);
    if (pDetour == NULL) {
        luaL_error(L, "invalid params count %d with call type %s", nparams, call_type_str(calltype));
    }
    hook *h = (hook*)lua_newuserdata(L, sizeof(hook));
    MH_STATUS s;
    LPVOID target;
    if (calltype == vtblhook_type) {
        target = (LPVOID)luaL_checkinteger(L, 1);
        s = MH_CreateHookFuncTable((LPVOID)target, pDetour, &pOriginal, h);
    } else {
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
    }
    if (s == MH_OK) {
        h->nparams = nparams;
        h->calltype = calltype;
        h->pOriginal = pOriginal;
        h->pTarget = (LPVOID)target;
        h->Ecx = 0;
        h->result = 0;
        h->hasResult = false;
        h->L = L;
        if (hasCallbackFunc) {
            lua_pushvalue(L, lua_isnumber(L, 3) ? 4 : 3);
            h->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
        } else {
            h->callbackRef = -1;
        }
        minhook_meta(L);
        register_hook(h);
        return 1;
    }
    lua_pop(L, 1);
    return 0;
}


static int lrawcreatehook(lua_State *L) {
    int nparams = (int)luaL_checkinteger(L, 1);
    int calltype = (int)luaL_checkinteger(L, 2);
    LPVOID pOriginal = (LPVOID)luaL_checkinteger(L, 3);
    int ecx = 0;
    if (lua_gettop(L) > 3) {
        ecx = (int)luaL_checkinteger(L, 4);
    }
    LPVOID pDetour = get_cb(nparams, calltype);
    if (pDetour == NULL) {
        luaL_error(L, "invalid params count %d with call type %s", nparams, call_type_str(calltype));
    }
    
    hook *h = (hook*)lua_newuserdata(L, sizeof(hook));
    h->nparams = nparams;
    h->calltype = calltype;
    h->pOriginal = pOriginal;
    h->pTarget = 0;
    h->Ecx = ecx;
    h->result = 0;
    h->hasResult = false;
    h->L = L;
    h->callbackRef = -1;
    minhook_meta(L);
    register_hook(h);
    return 1;
}

static int lkmdfind(lua_State *L) {
    int startAddr = (int)luaL_checkinteger(L, 1);
    int nCount = (int)luaL_checkinteger(L, 2);
    const char *patn = luaL_checkstring(L, 3);
    bool hex_patn = true;
    if (lua_gettop(L) > 3) {
        hex_patn = lua_toboolean(L, 4);
    }
    print("startAddr=%08X,nCount=%08X,patn=%s", startAddr, nCount, patn);
    int pos = kmpFind::Find((const unsigned char *)startAddr, nCount, patn, hex_patn);
    if (pos >= 0) {
        print("lkmdfind ok: %08X", pos + startAddr);
        lua_pushinteger(L, pos + startAddr);
        return 1;
    }
    print("lkmdfind failed");
    return 0;
}

static int lcall_original_wrap(lua_State *L) {
    hook_wrap *h = (hook_wrap *)lua_touserdata(L, 1);
    return _call_original(L, h->pHook);
}

static int lsetresult_wrap(lua_State *L) {
    hook_wrap *ph = (hook_wrap *)lua_touserdata(L, 1);
    if (ph != NULL) {
        hook *h = ph->pHook;
        if (lua_gettop(L) < 2 || lua_isnil(L, 2)) {
            h->result = 0;
            h->hasResult = false;
        } else {
            h->result = (int)luaL_checkinteger(L, 2);
            h->hasResult = true;
        }
    }
    return 0;
}

static int lgetresult_wrap(lua_State *L) {
    hook_wrap *ph = (hook_wrap *)lua_touserdata(L, 1);
    if (ph != NULL && ph->pHook->hasResult) {
        lua_pushinteger(L, ph->pHook->result);
        return 1;
    }
    return 0;
}

static void minhook_wrap_meta(lua_State *L) {
    if (luaL_newmetatable(L, "minhook_wrap")) {
        luaL_Reg l[] = {
            { "setresult", lsetresult_wrap },
            { "getresult", lgetresult_wrap },
            { NULL, NULL },
        };
        luaL_newlib(L, l);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, lcall_original_wrap);
        lua_setfield(L, -2, "__call");
    }
    lua_setmetatable(L, -2);
}

static int lpack(lua_State *L) {
    hook *h = (hook *)lua_touserdata(L, 1);
    if (h != NULL) {
        lua_pushinteger(L, (int)h);
        return 1;
    }
    return 0;
}

static int lunpack(lua_State *L) {
    hook *h = (hook *)luaL_checkinteger(L, 1);
    if (h != NULL) {
        hook_wrap *ph = (hook_wrap*)lua_newuserdata(L, sizeof(hook_wrap));
        ph->pHook = h;
        minhook_wrap_meta(L);
        return 1;
    }
    return 0;
}

EXTERN int luaopen_minhook(lua_State *L) {
    //luaL_checkversion(L);
    luaL_Reg l[] = {
        { "initialize", linitialize },
        { "uninitialize", luninitialize },
        { "create", lcreatehook },
        { "rawcreate", lrawcreatehook },
        { "kmdfind", lkmdfind },
        { "showconsole", lshowconsole },
        { "hideconsole", lhideconsole },
        { "pack", lpack },
        { "unpack", lunpack },
        { "setresult", lsetresult_wrap },
        { "getresult", lgetresult_wrap },
        { NULL, NULL },
    };
    luaL_newlib(L, l);

    lua_pushinteger(L, cdecl_type);
    lua_setfield(L, -2, "CDECL");
    lua_pushinteger(L, stdcall_type);
    lua_setfield(L, -2, "STDCALL");
    lua_pushinteger(L, thiscall_type);
    lua_setfield(L, -2, "THISCALL");
    lua_pushinteger(L, vtblhook_type);
    lua_setfield(L, -2, "VTBLHOOK");

    lua_newtable(L); // hook register weak table
    lua_newtable(L); // metatable={}
    lua_pushliteral(L, "__mode");
    lua_pushliteral(L, "v");
    lua_rawset(L, -3); // metatable._mode='v'
    lua_setmetatable(L, -2); // setmetatable(hook register table, metatable)
    lua_setglobal(L, IDX_HOOK_REGISTER);

    return 1;
}
