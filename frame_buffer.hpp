
#ifndef __frame_buffer__included__
#define __frame_buffer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"
#include "texture.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class frame_buffer :
        public oo_extensions::non_copyable
    {
    private:
        GLuint _frameBufferId = GL_INVALID_INDEX;

    public:
        property_get (GlId, _frameBufferId);


    protected:
        bool _testValid() const;

    public:
        declare_ptr_alloc (frame_buffer)
        frame_buffer();

        void attachColorTexture (texture::ptr colorTexture);
        void use() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
