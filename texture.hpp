
#ifndef __texture__included__
#define __texture__included__

//----------------------------------------------------------------------------------------------------------------------

#include "resource.hpp"
#include "gl_bindable.hpp"
#include "gl_binding.hpp"
#include "math3D.hpp"

#include <map>

using math3d::vector3;

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
            nearest               =  (unsigned)  GL_NEAREST,
            linear                =  (unsigned)  GL_LINEAR,
            nearest_MipmapNearest =  (unsigned)  GL_NEAREST_MIPMAP_LINEAR,
            linear_MipmapNearest  =  (unsigned)  GL_LINEAR_MIPMAP_NEAREST,
            linear_MipmapLinear   =  (unsigned)  GL_LINEAR_MIPMAP_LINEAR,
            //anisotropic           =  (unsigned)  GL_TEXTURE_MAX_ANISOTROPY_EXT
        };


        enum pixel_format_t
        {
            rgb      =  (unsigned)  GL_RGB,
            rgba     =  (unsigned)  GL_RGBA,
            oneFloat =  (unsigned)  GL_FLOAT
        };


    private:
        GLuint _textureId = 0;
        GLenum _textureType = GL_TEXTURE_2D;

    public:
        property_get (glId, _textureId);
        property_get (glTarget, _textureType);

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
        static texture::ptr create3D (vector3<unsigned> dimensions, float *voxels);
        static texture::ptr create3D (vector3<unsigned> dimensions, uint8_t *voxels);

        void use() const;
        void filtering (filtering_t shrinkFilter, filtering_t expFilter = filtering_t::linear);
        void uvClamping();
        void uvRepeating ();
        void uvRepeating (bool uMode, bool vMode, bool tMode = false);
        void setupForShadowSampler();
        void saveToFile (const string &fileName);

        unsigned height() const;
        unsigned width()  const;

        bool isMultisample() const;
        bool isCubemap() const;
        bool isRegular() const;
        bool is3D() const;

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
