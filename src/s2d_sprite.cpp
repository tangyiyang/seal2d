#include "s2d_sprite.h"
#include "s2d_context.h"
#include "s2d_util.h"
#include "cJSON.h"

NS_S2D

void sprite_frame_cache::load(const char* json_atlas, const char* texture_file)
{
    texture* tex = util::load_texture(texture_file);

    file_entry* file = util::load_file(json_atlas, false);
    cJSON* root = cJSON_Parse((const char*)file->_buffer);
    cJSON* frames = cJSON_GetObjectItemCaseSensitive(root, "frames");
    if (frames) {
        cJSON* child = frames->child;
        while (child) {
            sprite_frame* f = new sprite_frame();
            const char* name = child->string;
            f->_name = name;

            cJSON* frame_obj = cJSON_GetObjectItemCaseSensitive(child, "frame");
            f->_frame.origin.x = cJSON_GetObjectItemCaseSensitive(frame_obj, "x")->valueint;
            f->_frame.origin.y = cJSON_GetObjectItemCaseSensitive(frame_obj, "y")->valueint;
            f->_frame.size.width = cJSON_GetObjectItemCaseSensitive(frame_obj, "w")->valueint;
            f->_frame.size.height = cJSON_GetObjectItemCaseSensitive(frame_obj, "h")->valueint;

            cJSON* size_obj = cJSON_GetObjectItemCaseSensitive(child, "sourceSize");
            f->_source_size.width = cJSON_GetObjectItemCaseSensitive(size_obj, "w")->valueint;
            f->_source_size.height = cJSON_GetObjectItemCaseSensitive(size_obj, "h")->valueint;

            cJSON* source_size_obj = cJSON_GetObjectItemCaseSensitive(child, "spriteSourceSize");
            f->_sprite_source_rect.origin.x = cJSON_GetObjectItemCaseSensitive(source_size_obj, "x")->valueint;
            f->_sprite_source_rect.origin.y = cJSON_GetObjectItemCaseSensitive(source_size_obj, "y")->valueint;
            f->_sprite_source_rect.size.width = cJSON_GetObjectItemCaseSensitive(source_size_obj, "w")->valueint;
            f->_sprite_source_rect.size.height = cJSON_GetObjectItemCaseSensitive(source_size_obj, "h")->valueint;

            f->_rotated = (cJSON_GetObjectItemCaseSensitive(child, "rotated")->type == cJSON_True);
            f->_trimmed = (cJSON_GetObjectItemCaseSensitive(child, "trimmed")->type == cJSON_True);

            f->_texture = tex;
            _cache[name] = f;

            child = child->next;
        }
    }

    cJSON_Delete(root);
    file->release();
}

sprite_frame* sprite_frame_cache::get(const char* name)
{
    return _cache[name];
}

void sprite_frame_cache::shutdown()
{
    std::map<std::string, sprite_frame*>::iterator it = _cache.begin();
    for (; it != _cache.end(); ++it) {
        it->second->release();
    }
    _cache.clear();
}

void sprite::init()
{
    node::init();
    texture_blend_protocol::init();
}

void sprite::set_texture_rect(texture* tex, const rect& r)
{
    S2D_ASSERT(tex);
    node::init();

    texture_blend_protocol::set_texture(tex);

    this->set_quad(r, tex);
    _size = r.size;
}

void sprite::set_texture_with_file(const char* tex_file)
{
    texture* tex = context::C()->_texture_cache->load(tex_file);

    texture_blend_protocol::set_texture(tex);

    this->set_quad(nullptr, tex);
    _size = tex->_size;
}

void sprite::set_texture_with_frame(sprite_frame* frame)
{
    S2D_ASSERT(frame);
    
    texture_blend_protocol::set_texture(frame->_texture);

    this->set_quad(frame, _texture);
    _size = frame->_source_size;
}

void sprite::set_quad(const rect& r, texture* tex)
{
    float tex_w = tex->_size.width;
    float tex_h = tex->_size.height;
    float w = r.size.width;
    float h = r.size.height;
    float x = r.origin.x;
    float y = r.origin.y;

    uint16_t left = (uint16_t)(x/tex_w * (float)S2D_TEX_COORD_MAX);
    uint16_t right = (uint16_t)((x+w)/tex_w * (float)S2D_TEX_COORD_MAX);
    uint16_t bottom = (uint16_t)(y/tex_h * (float)S2D_TEX_COORD_MAX);
    uint16_t top = (uint16_t)((y+h)/tex_h * (float)S2D_TEX_COORD_MAX);

    _quad[0].pos.x = 0;
    _quad[0].pos.y = 0;
    _quad[0].uv.u = left;
    _quad[0].uv.v = top;
    _quad[0].color = 0xffffffff;

    _quad[1].pos.x = w;
    _quad[1].pos.y = 0;
    _quad[1].uv.u = right;
    _quad[1].uv.v = top;
    _quad[1].color = 0xffffffff;

    _quad[2].pos.x = 0;
    _quad[2].pos.y = h;
    _quad[2].uv.u = left;
    _quad[2].uv.v = bottom;
    _quad[2].color = 0xffffffff;

    _quad[3].pos.x = w;
    _quad[3].pos.y = h;
    _quad[3].uv.u = right;
    _quad[3].uv.v = bottom;
    _quad[3].color = 0xffffffff;
}

void sprite::set_quad(sprite_frame* frame, texture* tex)
{
    if (frame) {
        // load part of the texture by the rect provided by frame.
        float tex_w = tex->_size.width;
        float tex_h = tex->_size.height;
        float x = frame->_frame.origin.x;
        float y = frame->_frame.origin.y;
        float ow = frame->_frame.size.width;
        float oh = frame->_frame.size.height;

        float w = ow;
        float h = oh;
        if (frame->_rotated) {
            std::swap(w, h);
        }

        uint16_t left = (uint16_t)(x/tex_w * (float)S2D_TEX_COORD_MAX);
        uint16_t right = (uint16_t)((x+w)/tex_w * (float)S2D_TEX_COORD_MAX);
        uint16_t bottom = (uint16_t)(y/tex_h * (float)S2D_TEX_COORD_MAX);
        uint16_t top = (uint16_t)((y+h)/tex_h * (float)S2D_TEX_COORD_MAX);
        
        /* Notice:
         *  quad[0] -> bottom-left
         *  quad[1] -> bottom-right
         *  quad[2] -> top-left
         *  quad[3] -> top-right
         * opengl texture is upside-down, so when we swap the bottom and the top.
         */
        if (!frame->_rotated) {
            _quad[0].uv.u = left;
            _quad[0].uv.v = top;
            _quad[1].uv.u = right;
            _quad[1].uv.v = top;
            _quad[2].uv.u = left;
            _quad[2].uv.v = bottom;
            _quad[3].uv.u = right;
            _quad[3].uv.v = bottom;
        } else {
            _quad[0].uv.u = left;
            _quad[0].uv.v = bottom;
            _quad[1].uv.u = left;
            _quad[1].uv.v = top;
            _quad[2].uv.u = right;
            _quad[2].uv.v = bottom;
            _quad[3].uv.u = right;
            _quad[3].uv.v = top;
        }

        _quad[0].pos.x = 0;
        _quad[0].pos.y = 0;
        _quad[0].color = 0xffffffff;

        _quad[1].pos.x = ow;
        _quad[1].pos.y = 0;
        _quad[1].color = 0xffffffff;

        _quad[2].pos.x = 0;
        _quad[2].pos.y = oh;
        _quad[2].color = 0xffffffff;

        _quad[3].pos.x = ow;
        _quad[3].pos.y = oh;
        _quad[3].color = 0xffffffff;
    } else {
        
        // load the whole texture
        uint16_t left = 0;
        uint16_t right = S2D_TEX_COORD_MAX;
        uint16_t bottom = 0;
        uint16_t top = S2D_TEX_COORD_MAX;
        
        if (!tex->_upside_down) {
            // for most cases opengl texture is upside down.
            // for rare cases, such as render-texture is not.
            std::swap(bottom, top);
        }
        /*  0------3
         *  |      |
         *  |      |
         *  1------2
         *
         */
        
        _quad[0].pos.x = 0;
        _quad[0].pos.y = 0;
        _quad[0].uv.u = left;
        _quad[0].uv.v = top;
        _quad[0].color = 0xffffffff;

        _quad[1].pos.x = 0;
        _quad[1].pos.y = tex->_size.height;
        _quad[1].uv.u = left;
        _quad[1].uv.v = bottom;
        _quad[1].color = 0xffffffff;

        _quad[2].pos.x = tex->_size.width;
        _quad[2].pos.y = 0;
        _quad[2].uv.u = right;
        _quad[2].uv.v = top;
        _quad[2].color = 0xffffffff;

        _quad[3].pos.x = tex->_size.width;
        _quad[3].pos.y = tex->_size.height;
        _quad[3].uv.u = right;
        _quad[3].uv.v = bottom;
        _quad[3].color = 0xffffffff;
    }
}

bool sprite::update(float dt)
{
    return node::update(dt);
}

void sprite::draw(render_state* rs)
{
    if (!_visible) {
        return;
    }
    _world_transform = transform_to(this->get_root());

    if (_texture) {
        rs->draw_sprite(this);
    }

    node::draw(rs);
}

NS_S2D_END
