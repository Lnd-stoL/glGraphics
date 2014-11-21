
#ifndef __texture__included__
#define __texture__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"
#include "gl_bindable.hpp"

#include <GL/glew.h>
#include <map>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class texture :
        public resource,
        public gl_bindable<texture>
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


        enum pixel_format_t
        {
            rgb      = GL_RGB,
            rgba     = GL_RGBA,
            oneFloat = GL_FLOAT
        };


    private:
        GLuint _textureId = GL_INVALID_INDEX;
        GLenum _textureType = GL_TEXTURE_2D;

    public:
        property_get (GlId, _textureId);
        property_get (GlTarget, _textureType);

    protected:
        bool _testValid() const;
        void _initGLTexture();
        texture();

    public:
        declare_ptr_alloc (texture)

        texture (const string &fileName);
        texture (std::map<string, string> cubeMapFaces);
        virtual ~texture();

        static texture::ptr createEmptyRgb (unsigned width, unsigned height);
        static texture::ptr createEmptyRgbMultisampled (unsigned width, unsigned height, unsigned samples);
        static texture::ptr createEmptyDepth (unsigned width, unsigned height);

        void use() const;
        void filtering (filtering_t shrinkFilter, filtering_t expFilter = filtering_t::linear);
        void setupForShadowSampler();
        void saveToFile (const string &fileName);

        unsigned getHeight() const;
        unsigned getWidth() const;

        bool isMultisample() const;
        bool isCubemap() const;
        bool isRegular() const;

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
