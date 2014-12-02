
#ifndef __render_resources_included__
#define __render_resources_included__

//----------------------------------------------------------------------------------------------------------------------

#include "gpu_program.hpp"
#include "exs3d.hpp"
#include "texture.hpp"
#include "font_resource.hpp"
#include "resource_manager.hpp"
#include "config_file.hpp"


namespace render
{
    class resources
    {
        resource_manager<gpu_program>      _gpuProgramsManager;
        resource_manager<exs3d_mesh>       _meshesManager;
        resource_manager<texture>          _texturesManager;
        resource_manager<font>             _fontsManager;
        resource_manager<vertex_shader>    _vertexShadersManager;
        resource_manager<fragment_shader>  _fragmentShadersManager;
        resource_manager<geometry_shader>  _geometryShadersManager;
        resource_manager<config_set>       _configFilesManager;


    public:
        property_ref (gpuProgramsManager,      _gpuProgramsManager)
        property_ref (exs3dMeshesManager,      _meshesManager)
        property_ref (texturesManager,         _texturesManager)
        property_ref (fontsManager,            _fontsManager)
        property_ref (fragmentShadersManager,  _fragmentShadersManager)
        property_ref (vertexShadersManager,    _vertexShadersManager)
        property_ref (geometryShadersManager,  _geometryShadersManager)
        property_ref (configFilesManager,      _configFilesManager)


    public:
        template<typename resource_t>
        typename resource_t::ptr requestFromFile (const string &fileName);

        void addFilesSearchLocation (const string &path);
        void releaseUnused();
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
