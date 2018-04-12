local node = require "seal2d.game_object.node"
local action = require "seal2d_action"
local scroll_view =require("seal2d.gui.scroll_view")

local list_view = class("list_view", function()
    return node.new()
end)

function list_view:ctor(opt)
    assert(opt and type(opt) == "table")
    assert(opt.mode == "row" or opt.mode == "col")
    assert(type(opt.width) == "number" and type(opt.height) == "number")
    assert(type(opt.n_cell) == "number" and opt.n_cell > 0)
    assert(type(opt.cell_create_func) == "function")

    local container = scroll_view.new {width = opt.width, height = opt.height}
    print_r(container)
    container:set_direction(opt.mode == "row" and scroll_view.HORIZONTAL or scroll_view.VERTICAL)
    self:add_child(container)

    local index = 1
    local cell = opt.cell_create_func(opt, index)
    local cell_width, cell_height = cell:get_size()

    local x, y, x_advance, y_advance
    if opt.mode == "row" then
        x, y = cell_width/2, cell_height/2
        x_advance, y_advance = cell_width, 0
    else
        x, y = cell_width/2, opt.height - cell_height/2
        x_advance, y_advance = 0, -cell_height
    end

    print("x, y = ", x ,y)
    print("width, height = ", opt.width, opt.height)

    cell:set_pos(x, y)
    container:add_content(cell)

    index = index + 1
    while index <= opt.n_cell do
        cell = opt.cell_create_func(opt, index)
        cell:set_pos(x, y)
        container:add_content(cell)
        x = x + x_advance
        y = y + y_advance
        index = index + 1
    end
end

return list_view