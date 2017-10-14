/****************************************************************************
 * Copyright (c) Yiyang Tang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * ****************************************************************************/

#ifndef s2d_bmfont_h
#define s2d_bmfont_h

#include "s2d_common.h"
#include "s2d_node.h"
#include "s2d_sprite.h"
#include "s2d_texture.h"

NS_S2D

typedef uint32_t bmfont_charc_id_t;
struct bmfont_character {
    uint32_t id;     /* 4 bytes should be enough for most cases. */
    int x, y, width, height;
    int xoffset, yoffset;
    int xadvance;
    int page; /* should be 0 all the time, cause we only support 1 page right now. */
    int chnl; /* not used right now. */
    char letter[8]; /* for implemention simplicty, use more bytes :), 4 bytes is enough for most cases. */
};

class bmfont_info : public ref_counter {
public:
    struct info {
        /* we only need the spacing right now
         * char face[BM_FACE_NAME_LEN];
         * size_t size;
         * int bold;
         * int italic;
         * char charset[CHARSET_NAME_LEN];
         * int unicode;
         * int stretchH;
         * int smooth;
         * int aa;
         * int padding[4];
         */
        struct spacing {
            int x;
            int y;
        } spacing;
    } info;

    struct common {
        int lineHeight;
        int base;
        int scaleW;
        int scaleH;
        int pages;
        int packed;
    } common;

    struct page {
        int id;
        std::string file; /* this is where we store the texture. */
    } page;

public:
    std::string font_file;
    std::map<bmfont_charc_id_t, bmfont_character*> _characters;
};

class bmfont_info_cache {
public:
    bmfont_info* load_font(const char* atlas_file_path);
    void shutdown();

private:
    void insert_search_path(const char* atlas_file_path);
    bmfont_info* load_font_from_buffer(const uint8_t* buffer);
    
private:
    std::map<std::string, bmfont_info*> _cache;
};

class bmfont : public node {
public:
    bmfont();

public:
    void init(const char* text, const char* font_atlas);
    virtual rect bounds_in(node* space) override;
    /*
     *  bmfont doesn't need hit_test for most cases.
     *
     *  virtual void hit_test(touch_handler* handler, touch_event* event);
     *  virtual void on_touch(touch_event* event);
     */
    
public:
    void set_text(const char* text);

private:
    void update_text(const char* new_text);
    void append_glyph(uint32_t codepoint, float *cursor_x, float *cursor_y);

private:
    bmfont_info* _font_info; /* font metrics info */
    texture*     _texture;
    std::string  _text;
};

NS_S2D_END

#endif