
#include "font_resource.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    font::font (string fileName)
    {
        if (!this->loadFromFile (fileName))
        {
            debug::log::println_err (mkstr ("failed to load font from file '", fileName, "'"));
        }

        resource::_loaded();
    }
}
