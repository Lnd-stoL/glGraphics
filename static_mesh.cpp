
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

    void elementary_shapes::quadXZ (vector<simple_vertex> &vertices, vector<unsigned short> &indices, float scale, float y)
    {
        static vector<simple_vertex> vertices_ { {{-scale, y, -scale}, {1, 1}},
                                                 {{ scale, y, -scale}, {0, 1}},
                                                 {{-scale, y,  scale}, {1, 0}},
                                                 {{ scale, y,  scale}, {0, 0}}};

        static vector<unsigned short> indices_ { 0, 1, 3, 3, 2, 0 };

        vertices.swap (vertices_);
        indices.swap (indices_);
    }


    void elementary_shapes::quadXY (vector<simple_vertex> &vertices, vector<unsigned short> &indices, float scale, float z)
    {
        static vector<simple_vertex> vertices_ {
                {{-scale, -scale, z}, {0, 1}},
                {{ scale, -scale, z}, {1, 1}},
                {{-scale,  scale, z}, {0, 0}},
                {{ scale,  scale, z}, {1, 0}}};

        static vector<unsigned short> indices_ { 0, 1, 3, 3, 2, 0 };

        vertices.swap (vertices_);
        indices.swap (indices_);
    }
}
