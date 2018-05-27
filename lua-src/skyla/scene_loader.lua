--[[
    This is a quick and dirty json format parser for scene.
]]

local skyla = require "skyla"
local util = require "skyla_util"
local cjson = require "cjson"
local go = require "skyla.game_object.go"

local scene_loader = {}

local function res_full_path(ctx, path)
    return string.format("%s", path)
end

local function set_node(node, ctx, opt)
    assert(node)

    if opt.visible ~= nil then
        node:set_visible(opt.visible)
    end

    if opt.scale then
        node:set_scale(opt.scale.x, opt.scale.y)
    end

    if opt.rotation then
        node:set_rotation(opt.rotation)
    end

    if opt.position then
        node:set_pos(opt.position.x, opt.position.y)
    end

    if opt.zorder then
        node:set_zorder(opt.zorder)
    end

    if opt.size then
        node:set_size(opt.size.width, opt.size.height)
    end

    if opt.anchor then
        node:set_anchor(opt.anchor.x, opt.anchor.y)
    end
end

local function set_sprite(sprite, ctx, opt)
    assert(sprite)
    set_node(sprite, ctx, opt)

    local t = opt.texture
    if t then
        if t.type == "full-image" then
            sprite:set_texture(res_full_path(ctx, t.file))
        elseif t.type == "atlas" then
            sprite:set_texture("#" .. t.sprite_frame_name)
        else
            assert(false, "unkown sprite texture type, file corrupted?")
        end
    end
end

local function load_node(...)
    local node = go.node.new()
    set_node(node, ...)
    node:on_touch(function(_, phase, x, y)
        printf("node, phase = %d, x = %.2f, y = %.2f", phase, x, y)
    end)
    return node
end

local function load_sprite(...)
    local sprite = go.sprite.new()
    set_node(sprite, ...)
    set_sprite(sprite, ...)
    sprite:on_touch(function(_, phase, x, y)
        printf("sprite, id = %d, phase = %d, x = %.2f, y = %.2f", sprite:get_id(), phase, x, y)
    end)
    return sprite
end

local load_funcs = {
    ["node"]   = load_node,
    ["sprite"] = load_sprite,
}

local function parse(ctx, opt)
    local t = opt["type"]
    local f = load_funcs[t]
    if not f then
        print(string.format("scene_loader: unsupported type: %s. "
                            .. "we would only load the node part.", t))
        f = load_node
    end
    local node = f(ctx, opt)

    local children = opt.children
    if children then
        for _, child_opt in ipairs(children) do
            local c = parse(ctx, child_opt)
            node:add_child(c)
        end
    end

    return node
end

function scene_loader.load_from_data(data)
    local graph = data
    local ctx = {
        ["version"] = graph.version,
        ["work_dir"] = graph.work_dir
    }

    local opt = graph.data.setup
    return parse(ctx, opt)
end

function scene_loader.load(file_path, cache)
    local data = util.load_file(file_path, cache)
    local graph = cjson.decode(data)
    load_from_data(graph)
end

return scene_loader