
#ifndef __texture__included__
#define __texture__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"

#include <GL/glew.h>
#include <SOIL/SOIL.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class texture : public resource
    {
    public:
        enum filtering_t
        {
            nearest               = GL_NEAREST,
            linear                = GL_LINEAR,
            nearest_MipmapNearest = GL_NEAREST_MIPMAP_LINEAR,
            linear_MipmapNearest  = GL_LINEAR_MIPMAP_NEAREST,
            linear_MipmapLinear   = GL_LINEAR_MIPMAP_LINEAR,
            anisotropic           = GL_TEXTURE_MAX_ANISOTROPY_EXT
        };

    private:
        GLuint _textureId = GL_INVALID_INDEX;

    public:
        property_get (GlId, _textureId);


    protected:
        bool _testValid() const;

    public:
        declare_ptr_alloc (texture)
        texture (const std::string &fileName);
        texture (unsigned width, unsigned height);
        virtual ~texture();

        void use() const;
        void filtering (filtering_t shrinkFilter, filtering_t expFilter = filtering_t::linear);
        void saveToFile (const string &fileName);

        unsigned getHeight() const;
        unsigned getWidth() const;

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
