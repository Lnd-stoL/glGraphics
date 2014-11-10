
#ifndef __font_resource__included__
#define __font_resource__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"

#include <GL/glew.h>
#include <SFML/Graphics.hpp>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class font :
        public sf::Font,
        public resource
    {
    public:
        declare_ptr_alloc (font)

        font (string fileName);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
