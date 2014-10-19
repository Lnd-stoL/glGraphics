
#include "frame_buffer.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void frame_buffer::use() const
    {
        glBindFramebuffer (GL_FRAMEBUFFER, _frameBufferId);
        debug::gl::test();
    }


    bool frame_buffer::_testValid() const
    {
        use();

        if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            debug::log::println_err ("framebuffer is not ok");
            return false;
        }

        return true;
    }


    frame_buffer::frame_buffer()
    {
        if (!GLEW_ARB_framebuffer_object)
        {
            debug::log::println_err ("GLEW_ARB_framebuffer_object is not supported; can't use FBO");
            return;
        }

        glGenFramebuffers (1, &_frameBufferId);
        //if (!_testValid()) return;

        use();
        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 900);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
    }


    void frame_buffer::attachColorTexture (texture::ptr colorTexture)
    {
        if (!_testValid())  return;
        colorTexture->filtering (texture::nearest, texture::nearest);
        colorTexture->use();
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->getGlId(), 0);

        //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        //glDrawBuffers (1, DrawBuffers);
        _testValid();
    }
}
