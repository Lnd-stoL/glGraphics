
#include "frame_buffer.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    gl_bindable_impl (frame_buffer)

    GLenum frame_buffer::drawBuffersSequential[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
        GL_COLOR_ATTACHMENT8,
        GL_COLOR_ATTACHMENT9,
        GL_COLOR_ATTACHMENT10,
        GL_COLOR_ATTACHMENT11,
        GL_COLOR_ATTACHMENT12,
        GL_COLOR_ATTACHMENT13,
        GL_COLOR_ATTACHMENT14,
        GL_COLOR_ATTACHMENT15
    };

//----------------------------------------------------------------------------------------------------------------------

    void frame_buffer::use() const
    {
        _bind();
        _setStates();
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
        auto colorTexture = texture::createEmptyRgb (_width, _height);
        attachColorTexture (colorTexture);
        return colorTexture;
    }


    void frame_buffer::useDefault()
    {
        _bindDefault();
        _setDefaultStates();
    }


    void frame_buffer::_bindDefault()
    {
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        gl_bindable<frame_buffer>::_bindDefault();
    }


    texture::ptr frame_buffer::attachDepthTexture()
    {
        auto depthTexture = texture::createEmptyDepth (_width, _height);
        attachDepthTexture (depthTexture);

        return depthTexture;
    }


    void frame_buffer::attachDepthTexture (texture::ptr depthTexture)
    {
        if (!_testValid())
        {
            debug::log::println_err ("can't attach depth texture to invalid frame buffer");
            return;
        }

        if (_hasDepthBuffer)
        {
            debug::log::println_err ("can't attach two depth textures to a single frame buffer");
            return;
        }

        _bind();
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->glId(), 0);

        _bindDefault();
        _hasDepthBuffer = true;
    }


    bool frame_buffer::supported()
    {
        return  true; // TODO: fix it
    }


    bool frame_buffer::readyForRender() const
    {
        _bind();
        return  glCheckFramebufferStatus (GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }


    void frame_buffer::_setStates() const
    {
        if (!_numColorBuffers)
        {
            glDrawBuffer (GL_NONE);
            glReadBuffer (GL_NONE);
        }

        else
        {
            glDrawBuffers (_numColorBuffers, drawBuffersSequential);
        }
    }


    void frame_buffer::_setDefaultStates()
    {
        glDrawBuffer (GL_BACK);
        glReadBuffer (GL_BACK);

        glDrawBuffers (1, drawBuffersSequential);
    }


    void frame_buffer::_bind() const
    {
        if (gl_bindable<frame_buffer>::isBoundNow())  return;

        glBindFramebuffer (GL_FRAMEBUFFER, _frameBufferId);
        glBindFramebuffer (GL_DRAW_FRAMEBUFFER, _frameBufferId);
        debug::gl::test();

        gl_bindable<frame_buffer>::_bindThis();
    }


    void frame_buffer::clear()
    {
        glClearColor (_clearColor.r(), _clearColor.g(), _clearColor.b(), 1);

        glbinding::SharedBitfield<AttribMask, ClearBufferMask>  flags = 0;
        if (_numColorBuffers)  flags = flags | GL_COLOR_BUFFER_BIT;
        if (_hasDepthBuffer)   flags = flags | GL_DEPTH_BUFFER_BIT;

        //unsigned flags = 0;
        //if (_numColorBuffers)  flags = flags | GL_COLOR_BUFFER_BIT;
        //if (_hasDepthBuffer)   flags = flags | GL_DEPTH_BUFFER_BIT;

        glClear (flags);
    }


    void frame_buffer::clearColor (const color_rgb<float> &color)
    {
        _clearColor = color;
    }


    void frame_buffer::attachColorTexture (texture::ptr txt)
    {
        if (!_testValid())
        {
            debug::log::println_err ("can't attach color texture to invalid frame buffer");
            return;
        }

        GLint maxColorAttachements = 0;
        glGetIntegerv (GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachements);
        if (_numColorBuffers >= maxColorAttachements || _numColorBuffers >= maxPossibleColorAttachements)
        {
            debug::log::println_err (mkstr ("frame_buffer: too many color attachements; maximum supported is ", maxColorAttachements));
            return;
        }

        _bind();
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _numColorBuffers, txt->glTarget(), txt->glId(), 0);
        debug::gl::test();
        ++_numColorBuffers;

        _bindDefault();
    }


    texture::ptr frame_buffer::attachColorMultisampleTexture (unsigned samples)
    {
        auto txt = texture::createEmptyRgbMultisampled (_width, _height, samples);
        attachColorTexture (txt);
        return txt;
    }
}
