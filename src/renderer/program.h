/*
 * Shader Programs, with serveral embeded programs.
 * Author: yiyang
 * Copyright: see copyright at skyla.h
 */

#ifndef skyla_program_h
#define skyla_program_h

#include "skyla_common.h"
#include "core/util.h"

NS_SKYLA

class program : public ref_counter {
public:
    program() : _program_handle(0) {}

    enum EMBEDED_PROGRAMS {
        EMBEDED_PROGRAM_SPRITE_DEFAULT = 0,
        EMBEDED_PROGRAM_LINE_DEFAULT,

        EMBEDED_PROGRAM_MAX
    };

    enum VERTEX_ATTR {
        VERTEX_ATTR_POS = 0,
        VERTEX_ATTR_TEX_COORD,
        VERTEX_ATTR_COLOR,

        VERTEX_ATTR_COUNT,
    };

    enum UNIFORM_TYPE {
        UNIFORM_TYPE_1_F = 0,
        UNIFORM_TYPE_MATRIX_3_FV,

        UNIFORM_TYPE_MAX,
    };

public:
    static program* load_default_program(EMBEDED_PROGRAMS type);
    static GLuint   load_shader(GLenum shader_type, const char* shader_data);
    static void     bind_vertex_locations(GLuint program);
    static GLuint   load_program(GLuint vs, GLuint fs);

public:
    void init(GLuint vs, GLuint fs);
    void shutdown();

    void set_uniform(const char* name, UNIFORM_TYPE type, float* value, bool transpose = false);

    inline void use()
    {
        SKYLA_ASSERT(_program_handle > 0);
        glUseProgram(_program_handle);
    }

    inline void unuse()
    {
        glUseProgram(0);
    }

    inline GLuint get_handle()
    {
        return _program_handle;
    }

private:
    GLuint _program_handle;
    std::map<std::string, GLint> _map_uniform_location;
};

NS_SKYLA_END

#endif
