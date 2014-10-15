
#ifndef __rendererable__included__
#define __rendererable__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "camera.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class renderable
    {
    public:
        declare_ptr (renderable)
        virtual void draw (const camera &viewer) const = 0;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif


