
#include "texture.hpp"
#include "debug.hpp"

#include <SFML/Graphics.hpp>
#include <SOIL/SOIL.h>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    gl_bindable_impl (texture)


    texture::texture (const std::string &fileName)
    {
        unsigned flags = /*SOIL_FLAG_COMPRESS_TO_DXT |*/ SOIL_FLAG_DDS_LOAD_DIRECT;
        //unsigned flags = 0;
        _textureId = SOIL_load_OGL_texture (fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);

        //sf::Texture txt;
        //txt.loadFromFile (fileName);

        if (_textureId == 0)
        {
            debug::log::println_err (mkstr ("error loading texture: ", SOIL_last_result()));
            debug::gl::test();
            _textureId = GL_INVALID_INDEX;
        }

        use();
        debug::log::println ("generating mipmaps for the texture ...");
        glGenerateMipmap (_textureType);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) GL_REPEAT);

        resource::_loaded();
        debug::log::println_mrk (mkstr (asString(), " successfully loaded and is ready for use"));
    }


    /*virtual*/ texture::~texture()
    {
        if (_textureId && _textureId != GL_INVALID_INDEX)
        {
            glDeleteTextures (1, &_textureId);
            debug::gl::test();
            debug::log::println_gl (mkstr (asString(), " released"));
        }
    }


    string texture::asString() const
    {
        return mkstr ("texture [with id ", _textureId, "]");
    }


    void texture::use() const
    {
        if (gl_bindable<texture>::isBoundNow())  return;

        if (!_testValid()) return;
        glBindTexture (_textureType, _textureId);

        gl_bindable<texture>::_bindThis();
    }


    bool texture::_testValid() const
    {
        if (!_textureId || _textureId == GL_INVALID_INDEX)
        {
            debug::log::println_err (mkstr ("can't operate ", asString(), " ; it is in invalid state"));
            debug::gl::test();
            return false;
        }

        return true;
    }


    void texture::filtering (texture::filtering_t shrinkFilter, texture::filtering_t expFilter)
    {
        use();

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint) expFilter);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint) shrinkFilter);
        debug::gl::test();
    }


    texture::texture()
    {
        _initGLTexture();
    }


    void texture::saveToFile (const string &fileName)
    {
        debug::log::println (mkstr ("saving ", asString(), " to file '", fileName, "'"));

        unsigned width = getWidth();
        unsigned height = getHeight();

        GLbyte *imageData = new GLbyte[width * height * 4];
        glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        debug::gl::test();

        sf::Image image;
        image.create (width, height, (const sf::Uint8 *) imageData);
        image.saveToFile (fileName);
    }


    unsigned texture::getWidth() const
    {
        use();
        int width = 0;
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        return (unsigned) width;
    }


    unsigned texture::getHeight() const
    {
        use();
        int height = 0;
        glGetTexLevelParameteriv (GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        return (unsigned) height;
    }


    texture::ptr texture::createEmptyRgb (unsigned width, unsigned height)
    {
        auto txt = new texture();

        txt->use();
        glTexImage2D (GL_TEXTURE_2D, 0, (GLint) GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        debug::gl::test();

        txt->filtering (texture::linear, texture::linear);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) GL_CLAMP_TO_EDGE);
        debug::gl::test();

        return texture::ptr (txt);
    }


    void texture::_initGLTexture()
    {
        glGenTextures (1, &_textureId);
    }


    texture::ptr texture::createEmptyDepth (unsigned width, unsigned height)
    {
        auto txt = new texture();

        txt->use();
        glTexImage2D (GL_TEXTURE_2D, 0, (GLint) GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        debug::gl::test();

        txt->filtering (texture::linear, texture::linear);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) GL_CLAMP_TO_EDGE);

        return texture::ptr (txt);
    }


    void texture::setupForShadowSampler()
    {
        this->use();

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, (GLint) GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, (GLint) GL_LEQUAL);
    }


    texture::texture (std::map<string, string> cubeMapFaces)
    {
        unsigned flags = SOIL_FLAG_COMPRESS_TO_DXT;

        _textureId =
                SOIL_load_OGL_cubemap (cubeMapFaces["+X"].c_str(),
                                       cubeMapFaces["-X"].c_str(),
                                       cubeMapFaces["+Y"].c_str(),
                                       cubeMapFaces["-Y"].c_str(),
                                       cubeMapFaces["+Z"].c_str(),
                                       cubeMapFaces["-Z"].c_str(),
                                       SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);

        if (_textureId == 0)
        {
            debug::log::println_err (mkstr ("error loading texture: ", SOIL_last_result()));
            debug::gl::test();
            _textureId = GL_INVALID_INDEX;
        }

        _textureType = GL_TEXTURE_CUBE_MAP;

        use();
        glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (GLint) GL_LINEAR);
        glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (GLint) GL_LINEAR);
        glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (GLint) GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (GLint) GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (GLint) GL_CLAMP_TO_EDGE);

        debug::log::println_mrk (mkstr (asString(), " successfully loaded and is ready for use (cubemap; 6 faces)"));
    }


    texture::ptr texture::createEmptyRgbMultisampled (unsigned width, unsigned height, unsigned samples)
    {
        auto txt = new texture();
        txt->_textureType = GL_TEXTURE_2D_MULTISAMPLE;

        txt->use();
        glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB8, width, height, GL_FALSE);
        debug::gl::test();

        txt->filtering (texture::linear, texture::linear);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint) GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint) GL_CLAMP_TO_EDGE);
        debug::gl::test();

        return texture::ptr (txt);
    }


    bool texture::isMultisample() const
    {
        return _textureType == GL_TEXTURE_2D_MULTISAMPLE;
    }


    bool texture::isCubemap() const
    {
        return _textureType == GL_TEXTURE_CUBE_MAP;
    }


    bool texture::isRegular() const
    {
        return _textureType == GL_TEXTURE_2D;
    }


    bool texture::is3D() const
    {
        return _textureType == GL_TEXTURE_3D;
    }


    texture::ptr  texture::create3D (vector3<unsigned> dimensions, float *voxels)
    {
        glEnable (GL_TEXTURE_3D);

        auto txt = new texture();
        txt->_textureType = GL_TEXTURE_3D;

        txt->use();
        glTexImage3D (txt->_textureType, 0, (GLint) GL_RED,
                      dimensions.getX(), dimensions.getY(), dimensions.getZ(), 0,
                      GL_RED, GL_FLOAT, voxels);
        debug::gl::test();
        glGenerateMipmap (GL_TEXTURE_3D);

        txt->filtering (texture::linear, texture::linear);
        txt->uvRepeating();
        //glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        debug::gl::test();

        debug::log::println (mkstr ("created 3d float texture ", dimensions.asString()));
        return texture::ptr (txt);
    }


    texture::ptr  texture::create3D (vector3<unsigned> dimensions, uint8_t *voxels)
    {
        glEnable (GL_TEXTURE_3D);

        auto txt = new texture();
        txt->_textureType = GL_TEXTURE_3D;

        txt->use();
        glTexImage3D (txt->_textureType, 0, (GLint) GL_RED,
                      dimensions.getX(), dimensions.getY(), dimensions.getZ(), 0,
                      GL_RED, GL_UNSIGNED_BYTE, voxels);
        debug::gl::test();
        //glGenerateMipmap (txt->_textureType);

        txt->filtering (texture::linear, texture::linear);
        txt->uvRepeating();
        glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);

        debug::log::println (mkstr ("created 3d unsigned byte texture ", dimensions.asString()));
        return texture::ptr (txt);
    }


    void texture::uvRepeating()
    {
        glTexParameteri (_textureType, GL_TEXTURE_WRAP_S, (GLint) GL_REPEAT);
        glTexParameteri (_textureType, GL_TEXTURE_WRAP_T, (GLint) GL_REPEAT);
        if (is3D())  glTexParameteri (_textureType, GL_TEXTURE_WRAP_R, (GLint) GL_REPEAT);
    }


    void texture::uvClamping()
    {
        glTexParameteri (_textureType, GL_TEXTURE_WRAP_S, (GLint) GL_CLAMP_TO_EDGE);
        glTexParameteri (_textureType, GL_TEXTURE_WRAP_T, (GLint) GL_CLAMP_TO_EDGE);
        if (is3D())  glTexParameteri (_textureType, GL_TEXTURE_WRAP_R, (GLint) GL_CLAMP_TO_EDGE);
    }


    void texture::uvRepeating (bool uMode, bool vMode, bool tMode)
    {
        glTexParameteri (_textureType, GL_TEXTURE_WRAP_S, (GLint) (uMode ? GL_REPEAT : GL_CLAMP_TO_EDGE));
        glTexParameteri (_textureType, GL_TEXTURE_WRAP_T, (GLint) (vMode ? GL_REPEAT : GL_CLAMP_TO_EDGE));
        if (is3D())  glTexParameteri (_textureType, GL_TEXTURE_WRAP_R, (GLint) (tMode ? GL_REPEAT : GL_CLAMP_TO_EDGE));
    }
}
