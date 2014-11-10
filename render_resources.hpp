
#ifndef __render_resources_included__
#define __render_resources_included__

//----------------------------------------------------------------------------------------------------------------------

#include "gpu_program.hpp"
#include "exs3d.hpp"
#include "texture.hpp"
#include "font_resource.hpp"


namespace render
{
    class resources
    {
        resource_manager<gpu_program> _gpuProgramsManager;
        resource_manager<exs3d_mesh>  _meshesManager;
        resource_manager<texture>     _texturesManager;
        resource_manager<font>        _fontsManager;

    public:
        property_ref (gpuProgramsManager, _gpuProgramsManager)
        property_ref (meshesManager,      _meshesManager)
        property_ref (texturesManager,    _texturesManager)
        property_ref (fontsManager,       _fontsManager)
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
