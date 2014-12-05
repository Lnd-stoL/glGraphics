
#ifndef __font_resource__included__
#define __font_resource__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"
#include "texture.hpp"

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H


//----------------------------------------------------------------------------------------------------------------------

namespace render
{
//----------------------------------------------------------------------------------------------------------------------

    class resources;

//----------------------------------------------------------------------------------------------------------------------

    class font : public resource
    {
        texture::ptr _fontBitmap;

    public:
        property_get (bitmap, _fontBitmap)

    public:
        declare_ptr_alloc (font)

        font (string fileName, resources &renderRes);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
