
#ifndef __render_resources_impl__included__
#define __render_resources_impl__included__

//----------------------------------------------------------------------------------------------------------------------

#include "render_resources.hpp"
#include "resource_manager_impl.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<>
    exs3d_mesh::ptr resources::requestFromFile<exs3d_mesh> (const string &fileName)
    {
        return _meshesManager.request (fileName, *this);
    }


    template<>
    texture::ptr resources::requestFromFile<texture> (const string &fileName)
    {
        return _texturesManager.request (fileName);
    }


    /*template<>
    font::ptr resources::requestFromFile<font> (const string &fileName)
    {
        return _fontsManager.request (fileName);
    }*/
}

//----------------------------------------------------------------------------------------------------------------------

#endif
