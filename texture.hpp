
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
        GLuint _textureId = GL_INVALID_INDEX;

    public:
        property_get (GlId, _textureId);


    protected:
        bool _testValid() const;

    public:
        declare_ptr_alloc (texture)
        texture (const std::string &fileName);
        virtual ~texture();

        void bind() const;

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
