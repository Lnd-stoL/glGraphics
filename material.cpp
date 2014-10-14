
#include "material.hpp"

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
