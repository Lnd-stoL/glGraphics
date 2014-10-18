
#include "texture.hpp"
#include "debug.hpp"

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


    void texture::bind() const
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
            return false;
        }

        return true;
    }
}
