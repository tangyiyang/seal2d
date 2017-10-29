#include "s2d_lua_context.h"
#include "s2d_util.h"
#include "s2d_sprite.h"
#include "s2d_context.h"

#ifdef S2D_ENABLE_LUA

#ifdef __cplusplus
extern "C" {
extern int luaopen_cjson(lua_State* L);
}
#endif

#define SEAL2D_TYPE_SPRITE "seal2d.sprite"

NS_S2D

#define TRACE_BACK_FUNC_INDEX (1)

static int traceback (lua_State* L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg) {
        luaL_traceback(L, L, msg, 1);
    } else if (!lua_isnoneornil(L, 1)) {
        if (!luaL_callmeta(L, 1, "__tostring")) {
            lua_pushliteral(L, "(no error message)");
        }
    }
    return 1;
}

// TODO: add the binding type checking, we should consider the inheritance checking, sprite is valid for node.
static bool type_valid(lua_State* L, const char* expected)
{
#if ENABLE_TYPE_CHECK
    lua_getmetatable(L, 1);
    lua_getfield(L, -1, "__type");
    const char* type = lua_tostring(L, -1);
    if (strcmp(type, expected) != 0) {
        luaL_error(L, "lseal2d_sprite_set_pos, require: %s, but got: %s", expected, type);
        return false;
    }
    lua_pop(L, 2);
    return true;
#endif
}

static int lseal2d_inject(lua_State* L)
{
    if(!lua_istable(L, -1)) {
        LOGE("seal2d lua require a functions of {on_init, on_update, on_destroy} to start. :)");
        exit(1);
    }

    lua_getfield(L, -1, "on_start");
    lua_setfield(L, LUA_REGISTRYINDEX, CONTEXT_START);
    lua_getfield(L, -1, "on_update");
    lua_setfield(L, LUA_REGISTRYINDEX, CONTEXT_UPDATE);
    lua_getfield(L, -1, "on_destory");
    lua_setfield(L, LUA_REGISTRYINDEX, CONTEXT_DESTROY);
    lua_pop(L, -1);
    return 0;
}


static int lseal2d_context_get_root(lua_State* L)
{
    context* C = (context*)lua_touserdata(L, 1);
    lua_pushlightuserdata(L, C->get_root());
    return 1;
}

static int lseal2d_context_get_visible_rect(lua_State* L)
{
    context* C = (context*)lua_touserdata(L, 1);

    rect* r = &(C->_visible_rect);
    lua_newtable(L);
    lua_pushnumber(L, r->origin.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, r->origin.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, r->size.width);
    lua_setfield(L, -2, "width");
    lua_pushnumber(L, r->size.height);
    lua_setfield(L, -2, "height");

    return 1;
}

static int lseal2d_context(lua_State* L)
{
    // TODO: fix me!
    LOGE("fix the lseal2d_context!!");
    static context* c = context::C();
    lua_pushlightuserdata(L, c);
    if (luaL_newmetatable(L, "SEAL2D_CONTEXT")) {
        luaL_Reg l[] = {
            { "get_root", lseal2d_context_get_root },
            { "get_visible_rect", lseal2d_context_get_visible_rect },
            { NULL, NULL },
        };
        luaL_newlib(L, l);
        lua_setfield(L, -2, "__index");
        lua_pushstring(L, "seal2d.context");
        lua_setfield(L, -2, "__type");

        lua_setmetatable(L, -2);
    }

    lua_context::stackDump(L);
    return 1;
}

static int lseal2d_node_add_child(lua_State* L)
{
    node* n = (node*)lua_touserdata(L, 1);
    node* child = (node*)lua_touserdata(L, 2);

    n->add_child(child);
    return 0;
}

static int lseal2d_node_set_visible(lua_State* L)
{
    node* n = (node*)lua_touserdata(L, 1);
    bool visible = lua_toboolean(L, 2);
    n->set_visible(visible);
    return 0;
}

static int lseal2d_node_set_size(lua_State* L)
{
    node* n = (node*)lua_touserdata(L, 1);
    lua_Number width = luaL_checknumber(L, 2);
    lua_Number height = luaL_checknumber(L, 3);
    n->set_size(width, height);
    return 0;
}

static int lseal2d_node_set_pos(lua_State* L)
{
    node* n = (node*)lua_touserdata(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);
    n->set_pos(x, y);
    return 0;
}

static int lseal2d_node_set_anchor(lua_State* L)
{
    node* n = (node*)(lua_touserdata(L, 1));
    lua_Number ax = luaL_checknumber(L, 2);
    lua_Number ay = luaL_checknumber(L, 2);
    n->set_anchor(ax, ay);
    return 0;
}

static int lseal2d_node_set_rotation(lua_State* L)
{
    node* n = (node*)(lua_touserdata(L, 1));
    lua_Number r = luaL_checknumber(L, 2);
    n->set_rotation(r);
    return 0;
}

static int lseal2d_node_set_scale(lua_State* L)
{
    int n_args = lua_gettop(L);
    if (n_args == 2) {
        node* n = (node*)(lua_touserdata(L, 1));
        lua_Number scale = luaL_checknumber(L, 2);
        n->set_scale(scale);
    } else if (n_args == 3) {
        node* n = (node*)(lua_touserdata(L, 1));
        lua_Number x = luaL_checknumber(L, 2);
        lua_Number y = luaL_checknumber(L, 3);
        n->set_scale(x, y);
    } else {
        luaL_error(L, "node.set_scale has got wrong number of arguments, expected 2 or 3, but got %d", n_args);
    }

    return 0;
}

static luaL_Reg node_funcs[] = {
    { "add_child", lseal2d_node_add_child },
    { "set_visible", lseal2d_node_set_visible},
    { "set_pos", lseal2d_node_set_pos },
    { "set_anchor", lseal2d_node_set_anchor },
    { "set_rotation", lseal2d_node_set_rotation },
    { "set_scale", lseal2d_node_set_scale },
    { "set_size", lseal2d_node_set_size },
};

static int lseal2d_sprite_set_color(lua_State* L)
{
    sprite* s = (sprite*)lua_touserdata(L, 1);
    lua_Integer color = luaL_checkinteger(L, 2);
    s->set_color((uint32_t)color);
    return 0;
}


static luaL_Reg sprite_funcs[] = {
    { "set_color", lseal2d_sprite_set_color     },
    { "set_texture", lseal2d_sprite_set_texture },
};

static int lseal2d_new_node(lua_State* L)
{
    node* n = new node();
    lua_pushlightuserdata(L, n);
    if (luaL_newmetatable(L, "SEAL2D_NODE")) {
        int n = sizeof(node_funcs)/sizeof(luaL_Reg) + sizeof(sprite_funcs)/sizeof(luaL_Reg);
        struct luaL_Reg* funcs = (luaL_Reg*)malloc(sizeof(luaL_Reg) * (n+1));
        int index = 0;
        for (int i = 0; i < sizeof(node_funcs)/sizeof(luaL_Reg); ++i) {
            funcs[index++] = node_funcs[i];
        }

        funcs[index] = {NULL, NULL};
        lua_createtable(L, 0, index);
        luaL_setfuncs(L, funcs, 0);
        lua_setfield(L, -2, "__index");
        free(funcs);

        lua_pushstring(L, "seal2d.node");
        lua_setfield(L, -2, "__type");

        lua_context::stackDump(L);
    }
    lua_setmetatable(L, -2);

    lua_context::stackDump(L);
    return 1;
}

static int lseal2d_new_sprite(lua_State* L)
{
    int n = lua_gettop(L);

    sprite* s = nullptr;
    if (n == 1) {
        s = new sprite();
        char* file_name = (char*)luaL_checkstring(L, 1);
        if (file_name[0] == '#') {
            sprite_frame* f = context::C()->_sprite_frame_cache->get(file_name+1);
            s->init(f);
        } else {
            s->init(file_name);
        }
    } else if (n == 0) {
        s = new sprite();
        s->init();
    } else {
        luaL_error(L, "invalid arguments to seal2d.new_sprite");
        return 0;
    }

    lua_pushlightuserdata(L, s);
    if (luaL_newmetatable(L, "SEAL2D_SPRITE")) {
        int n = sizeof(node_funcs)/sizeof(luaL_Reg) + sizeof(sprite_funcs)/sizeof(luaL_Reg);
        struct luaL_Reg* funcs = (luaL_Reg*)malloc(sizeof(luaL_Reg) * (n+1));
        int index = 0;
        for (int i = 0; i < sizeof(node_funcs)/sizeof(luaL_Reg); ++i) {
            funcs[index++] = node_funcs[i];
        }
        for (int i = 0; i < sizeof(sprite_funcs)/sizeof(luaL_Reg); ++i) {
            funcs[index++] = sprite_funcs[i];
        }
        funcs[index] = {NULL, NULL};
        lua_createtable(L, 0, index);
        luaL_setfuncs(L, funcs, 0);
        lua_setfield(L, -2, "__index");
        free(funcs);

        lua_pushstring(L, "seal2d.sprite");
        lua_setfield(L, -2, "__type");
    }

    lua_setmetatable(L, -2);
    return 1;
}

int lseal2d_util_load_file(lua_State* L)
{
    const char* file_path = luaL_checkstring(L, 1);
    bool cached = lua_toboolean(L, 2);

    file_entry* f = util::load_file(file_path, cached);

    lua_pushlstring(L, (const char*)f->_buffer, f->_size);
    return 1;
}

static int luaopen_seal2d_util(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif

    luaL_Reg lib[] = {
        { "load_file",  lseal2d_util_load_file },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}

static int luaopen_seal2d(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif

    luaL_Reg lib[] = {
        { "inject",  lseal2d_inject     },
        { "context", lseal2d_context    },
        { "node",    lseal2d_new_node   },
        { "sprite",  lseal2d_new_sprite },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);
    return 1;
}

void lua_context::stackDump (lua_State* L)
{
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:  /* strings */
                LOGD_NO_NEW_LINE("%s ", lua_tostring(L, i));
                break;

            case LUA_TBOOLEAN:  /* booleans */
                LOGD_NO_NEW_LINE("%s ", lua_toboolean(L, i) ? "true" : "false");
                break;

            case LUA_TNUMBER:  /* numbers */
                LOGD_NO_NEW_LINE("%g ", lua_tonumber(L, i));
                break;

            default:  /* other values */
                LOGD_NO_NEW_LINE("%s ", lua_typename(L, t));
                break;
        }
    }
    LOGD_NO_NEW_LINE("\n");
}

int lua_context::call_lua(lua_State* L, int n, int r)
{
    int err = lua_pcall(L, n, r, TRACE_BACK_FUNC_INDEX);
    switch(err) {
        case LUA_OK:
            break;
        case LUA_ERRRUN:
            LOGE("!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
            assert(false);
            break;
        case LUA_ERRMEM:
            LOGE("!LUA_ERRMEM : %s\n", lua_tostring(L,-1));
            assert(false);
            break;
        case LUA_ERRERR:
            LOGE("!LUA_ERRERR : %s\n", lua_tostring(L,-1));
            assert(false);
            break;
        case LUA_ERRGCMM:
            LOGE("!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
            assert(false);
            break;
        default:
            LOGE("!Unknown Lua error : %s\n", lua_tostring(L,-1));
            assert(false);
            break;
    }
    return err;
}

void lua_context::register_lua_extensions()
{
    luaL_Reg lua_modules[] = {
        { "cjson",         luaopen_cjson       },
        { "seal2d",        luaopen_seal2d      },
        { "seal2d.util",   luaopen_seal2d_util },
        { NULL, NULL}
    };

    luaL_Reg* lib = lua_modules;
    lua_getglobal(_lua_state, "package");
    lua_getfield(_lua_state, -1, "preload");
    for (; lib->func; lib++) {
        lua_pushcfunction(_lua_state, lib->func);
        lua_setfield(_lua_state, -2, lib->name);
    }

    lua_pop(_lua_state, 2);
}

void lua_context::init()
{
    _lua_state = nullptr;
    _lua_state = luaL_newstate();
    lua_assert(_lua_state);

    luaL_openlibs(_lua_state);

    register_lua_extensions();

    assert(lua_gettop(_lua_state) == 0);
    lua_pushcfunction(_lua_state, traceback);
}

void lua_context::on_start(context* ctx, const char* script_path)
{
    lua_State* L = _lua_state;
    int r = luaL_loadfile(L, script_path);
    if (r != LUA_OK) {
        LOGE("error load lua file\n");
        return;
    }
    call_lua(L, 0, LUA_MULTRET);

    lua_getfield(L, LUA_REGISTRYINDEX, CONTEXT_START);
    call_lua(_lua_state, 0, 0);
}

void lua_context::on_update(uint32_t dt)
{
    lua_getfield(_lua_state, LUA_REGISTRYINDEX, CONTEXT_UPDATE);
    lua_pushinteger(_lua_state, dt);
    call_lua(_lua_state, 1, 0);
}

void lua_context::on_destroy()
{
    lua_getfield(_lua_state, LUA_REGISTRYINDEX, CONTEXT_DESTROY);
    call_lua(_lua_state, 0, 0);
}

NS_S2D_END

#endif
