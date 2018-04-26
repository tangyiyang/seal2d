/*
 * Sprite for most users, you may create a sprite by a whole texture or atlas.
 * Atlas is generated with TexturePacker: https://www.codeandweb.com/texturepacker
 * With the V-Play format.
 * Author: yiyang
 * Copyright: see copyright at s2d.h
 */

#ifndef s2d_sprite_h
#define s2d_sprite_h

#include "s2d_node.h"
#include "s2d_texture.h"
#include "s2d_program.h"
#include "s2d_render_state.h"

NS_S2D

class sprite_frame : public ref_counter {
public:
    rect _frame;
    rect _sprite_source_rect;
    size _source_size;
    bool _rotated;
    bool _trimmed;
    std::string _name;
    texture* _texture;
};

class sprite_frame_cache {
public:
    void load(const char* json_atlas, const char* texture_file);
    sprite_frame* get(const char* name);

    void shutdown();
private:
    std::map<std::string, sprite_frame*> _cache;
};

class sprite: public node,
              public texture_blend_protocol {
    /*
     * The Quad Sprite.
     */
public:
    void init() override;
                  
public:
    void set_texture(texture* tex);
    void set_texture_with_file(const char* tex_file);
    void set_texture_with_frame(sprite_frame* frame);
    void set_texture_rect(texture* tex, const rect& r);
                  
public:
    inline void set_color(uint32_t color)
    {
        for (int i = 0; i < 4; ++i) {
            _quad[i].color = color;
        }
    }

public:
    bool update(float dt)       override;
    void draw(render_state* rs) override;

protected:
    void update_srt();

private:
    void set_quad_with_rect(texture* tex, const rect& r);
    void set_quad_with_frame(texture* tex, sprite_frame* frame);

public:
    pos_tex_color_vertex _quad[4];
};

NS_S2D_END

#endif
