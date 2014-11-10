
#include "water_plane.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

void water_plane::draw (graphics_renderer &renderer) const
{

}


water_plane::water_plane (transform_d const &transform) : _transform (transform)
{
    //_mesh = mesh_component<elementary_meshes::simple_vertex, unsigned short>::alloc ()
}

