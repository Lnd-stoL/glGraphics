
#include "material.hpp"
#include "debug.hpp"

#include <GL/glew.h>
#include <fstream>
#include <exception>
#include <stdexcept>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    /*virtual*/ void material::use() const
    {
        _renderingProgram->use();
    }


    /*virtual*/ void textured_material::use() const
    {
        material::use();
        
        sf::Texture::bind (_texture.get());
    }
}
