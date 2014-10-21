
#include "frame_buffer.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    gl_bindable_impl (frame_buffer)


    void frame_buffer::use() const
    {
        if (gl_bindable<frame_buffer>::isBoundNow())  return;

        if (!_testValid())  return;
        glBindFramebuffer (GL_FRAMEBUFFER, _frameBufferId);
        debug::gl::test();

        gl_bindable<frame_buffer>::_bindThis();
    }


    bool frame_buffer::_testValid() const
    {
        if (_frameBufferId == 0 || _frameBufferId == GL_INVALID_INDEX)
        {
            debug::log::println_err ("framebuffer is not ok");
            return false;
        }

        return true;
    }


    frame_buffer::frame_buffer (unsigned width, unsigned height) : _width (width), _height (height)
    {
        if (!supported())
        {
            debug::log::println_err ("GLEW_ARB_framebuffer_object is not supported; can't use FBO with current driver");
            return;
        }

        glGenFramebuffers (1, &_frameBufferId);
        if (!_testValid()) return;
    }


    texture::ptr frame_buffer::attachColorTexture()
    {
        if (!_testValid())
        {
            debug::log::println_err ("can't attach color texture to invalid frame buffer");
            return texture::ptr();
        }

        auto colorTexture = texture::createEmptyRgb (_width, _height);
        colorTexture->use();

        use();
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture->getGlId(), 0);

        //GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        //glDrawBuffers (1, DrawBuffers);

        _testValid();
        useDefault();
        return colorTexture;
    }


    void frame_buffer::useDefault()
    {
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        gl_bindable<frame_buffer>::_bindDefault();
    }


    texture::ptr frame_buffer::attachDepthTexture()
    {
        if (!_testValid())
        {
            debug::log::println_err ("can't attach depth texture to invalid frame buffer");
            return texture::ptr();
        }

        auto depthTexture = texture::createEmptyDepth (_width, _height);
        use();
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->getGlId(), 0);

        _testValid();
        useDefault();
        return depthTexture;
    }


    bool frame_buffer::supported()
    {
        return  GLEW_ARB_framebuffer_object;
    }


    bool frame_buffer::readyForRender() const
    {
        use();
        return  glCheckFramebufferStatus (GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
}
