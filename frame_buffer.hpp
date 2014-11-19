
#ifndef __frame_buffer__included__
#define __frame_buffer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include "gl_bindable.hpp"
#include "material.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class frame_buffer :
        public oo_extensions::non_copyable,
        public gl_bindable<frame_buffer>
    {
    private:
        static const unsigned maxPossibleColorAttachements = 16;
        static GLenum drawBuffersSequential[maxPossibleColorAttachements];

        GLuint _frameBufferId = GL_INVALID_INDEX;

        unsigned  _width  = 0;
        unsigned  _height = 0;

        unsigned _numColorBuffers = 0;
        bool  _hasDepthBuffer = false;

        color_rgb<float>  _clearColor;


    public:
        property_get (GlId,   _frameBufferId);
        property_get (Width,  _width);
        property_get (Height, _height);


    protected:
        bool _testValid() const;
        void _setStates() const;
        static void _setDefaultStates();
        void _bind() const;
        static void _bindDefault();

    public:
        declare_ptr_alloc (frame_buffer)
        frame_buffer (unsigned width, unsigned height);

        texture::ptr attachColorTexture();
        void attachColorTexture (texture::ptr txt);
        texture::ptr attachDepthTexture();
        void attachDepthTexture (texture::ptr txt);

        void use() const;

        void clearColor (const color_rgb<float> &color);
        void clear();
        bool readyForRender() const;

        static void useDefault();
        static bool supported();
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
