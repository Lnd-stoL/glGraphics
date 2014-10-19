
#include "texture.hpp"
#include "debug.hpp"

#include <SFML/Graphics.hpp>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    texture::texture (const std::string &fileName)
    {
        unsigned flags = SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT;
        _textureId = SOIL_load_OGL_texture (fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);

        if (_textureId == 0)
        {
            debug::log::println_err (mkstr ("error loading texture: ", SOIL_last_result()));
            debug::gl::test();
            _textureId = GL_INVALID_INDEX;
        }

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
        if (!_testValid()) return;
        glBindTexture (GL_TEXTURE_2D, _textureId);
        debug::gl::test();
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

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, expFilter);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, shrinkFilter);
        debug::gl::test();
    }


    texture::texture (unsigned width, unsigned height)
    {
        glGenTextures (1, &_textureId);
        use();

        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        debug::gl::test();
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
}
