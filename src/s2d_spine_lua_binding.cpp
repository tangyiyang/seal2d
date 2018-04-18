#include "s2d_spine_lua_binding.h"
#include "s2d_spine.h"

#if (S2D_ENABLE_LUA == 1)

NS_S2D

int lua_seal2d_spine_new(lua_State* L)
{
    int n = lua_gettop(L);

    if (n == 2) {
        spine_anim* s = new spine_anim();
        const char* skeleton_file = luaL_checkstring(L, 1);
        const char* atlas_file = luaL_checkstring(L, 2);
        s->init(skeleton_file, atlas_file);
        lua_pushlightuserdata(L, s);
        return 1;
    } else {
        luaL_error(L, "invalid number of arguments for seal2d.spine.new, expected 2, got %d", n);
    }
    return 0;
}

int lua_seal2d_spine_set_anim(lua_State* L)
{
    int n = lua_gettop(L);
    if (n == 4) {
        lua_getfield(L, 1, "__cobj");
        spine_anim* n = (spine_anim*)lua_touserdata(L, -1);
        const char* name = luaL_checkstring(L, 2);
        int track = luaL_checkinteger(L, 3);
        int loop = luaL_checkinteger(L, 4);
        n->set_anim(name, track, loop);

        return 0;
    } else {
        luaL_error(L, "invalid number of arguments passed to sprite_set_blend_mode"
                   "expected more than 2, but got %d", n);
        return 0;
    }
}

int luaopen_seal2d_spine(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif

    luaL_Reg lib[] = {
        { "new",  lua_seal2d_spine_new },
        { "set_anim", lua_seal2d_spine_set_anim },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);
    return 1;
}

NS_S2D_END

#endif

