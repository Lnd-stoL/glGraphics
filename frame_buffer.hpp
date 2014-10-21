
#ifndef __frame_buffer__included__
#define __frame_buffer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"
#include "texture.hpp"
#include "gl_bindable.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class frame_buffer :
        public oo_extensions::non_copyable,
        public gl_bindable<frame_buffer>
    {
    private:
        GLuint _frameBufferId = GL_INVALID_INDEX;

        unsigned  _width  = 0;
        unsigned  _height = 0;

    public:
        property_get (GlId,   _frameBufferId);
        property_get (Width,  _width);
        property_get (Height, _height);


    protected:
        bool _testValid() const;

    public:
        declare_ptr_alloc (frame_buffer)
        frame_buffer (unsigned width, unsigned height);

        texture::ptr attachColorTexture();
        texture::ptr attachDepthTexture();

        void use() const;
        bool readyForRender() const;

        static void useDefault();
        static bool supported();
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
