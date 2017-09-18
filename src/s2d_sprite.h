/****************************************************************************
Copyright (c) Yiyang Tang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef s2d_sprite_h
#define s2d_sprite_h

#include "s2d_common.h"
#include "s2d_texture.h"
#include "s2d_vertex_buffer.h"

NS_S2D

#define USE_SPRITE_VERTEX

class sprite {
public:
    void init();
    void update();
    void draw();

    void update_srt();

public:


#ifdef USE_SPRITE_VERTEX
    pos_tex_color_vertex _quad[4];
    pos_tex_color_vertex _vertex[6];
#else
    vec2 _vertex[3];
    vec2 _buffer[3];
#endif

    GLuint _program;
    GLuint _vbo;
    GLuint _vao;
    GLint _u_projection;

    vec2 _pos;
    vec2 _scale;
    vec2 _anchor;
    size _size;
    float _rotation;

    affine_transform _local_transform;
    
    texture* _texture;
};

NS_S2D_END
#endif /* s2d_sprite_h */
