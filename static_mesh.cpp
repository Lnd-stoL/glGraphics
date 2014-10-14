
#include "mesh.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void static_mesh::addComponent (basic_mesh_component::ptr component)
    {
        _components.push_back (component);
    }


    void static_mesh::removeComponent (const string &name)
    {
        for (auto i = _components.begin(); i != _components.end(); ++i)
        {
            if ((*i)->getName() == name) i = _components.erase (i);
        }
    }


    void static_mesh::draw() const
    {
        for (auto component : _components)
        {
            component->draw();
        }
    }
}
