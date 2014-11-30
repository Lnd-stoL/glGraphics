
#ifndef __rendererable__included__
#define __rendererable__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class graphics_renderer;

//----------------------------------------------------------------------------------------------------------------------

    class renderable
    {
    protected:
        bool _visible = true;

    public:
        property_get (Visible, _visible)

    public:
        declare_ptr (renderable);

        virtual  void hide()           { _visible = false; }
        virtual  void makeVisible()    { _visible = true;  }

        virtual  void draw (graphics_renderer &renderer) const = 0;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
