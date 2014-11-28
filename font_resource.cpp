
#include "font_resource.hpp"

#include "debug.hpp"
#include "render_resources.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    font::font (string fileName, resources &renderRes)
    {
        _fontBitmap = renderRes.requestFromFile<texture> (fileName);
        resource::_loaded();
    }
}
