//
//  Created by yiyang.tang on 2017/9/11.
//

#include "s2d_gl_util.h"
#include "s2d_util.h"


void s2d_gl_util::check_gl_error(const char* file, int line)
{
    GLenum err = GL_NO_ERROR;
    if((err = glGetError()) != GL_NO_ERROR) {
        LOGE("gl error: %04x. file = %s, line = %d\n", err, file, line);
    }
}

void s2d_gl_util::check_gl_extension()
{
    const char* gl_vendor = (const char*)glGetString(GL_VENDOR);
    const char* gl_renderer = (const char*)glGetString(GL_RENDERER);
    const char* gl_version = (const char*)glGetString(GL_VERSION);
    const char* gl_extension = (const char *)glGetString(GL_EXTENSIONS);

    GLint n = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);

    for (int i = 0; i < n; ++i) {
        LOGD("%d: %s", i, (const char*)glGetStringi(GL_EXTENSIONS, i));
    }

    LOGD("gl_vendor = %s", gl_vendor);
    LOGD("gl_renderer = %s", gl_renderer);
    LOGD("gl_version = %s", gl_version);
    LOGD("gl_extension = %s", gl_extension);
}
