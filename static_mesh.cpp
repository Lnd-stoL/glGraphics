
#include "mesh.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void mesh::addComponent (a_mesh_component::ptr component)
    {
        _components.push_back (component);
    }


    void mesh::removeComponent (const string &name)
    {
        for (auto i = _components.begin(); i != _components.end(); ++i)
        {
            if ((*i)->getName() == name)  i = _components.erase (i);
        }
    }


    /*virtual*/ void mesh::draw (graphics_renderer &renderer) const
    {
        for (auto component : _components)
        {
            component->draw (renderer);
        }
    }

//----------------------------------------------------------------------------------------------------------------------

    void elementary_shapes::quad (vector<simple_vertex> &vertices, vector<unsigned short> &indices)
    {
        vector<simple_vertex> vertices_ { {{-1, 0, -1}}, {{1, 0, -1}}, {{-1, 0,  1}}, {{1, 0,  1}} };
        vector<unsigned short> indices_ { 0, 1, 3, 3, 2, 0 };

        vertices.swap (vertices_);
        indices.swap (indices_);
    }
}
