
#include "mesh.hpp"
#include "debug.hpp"

#include <GL/glew.h>
#include <fstream>
#include <exception>
#include <stdexcept>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename vertex_t, typename index_t>
    void mesh_component<vertex_t, index_t>::draw() const
    {
        _vertexBuffer->use();
        _indexBuffer->use();

        _material->use();

        glDrawElements (GL_TRIANGLES, _indexBuffer->getSize(), GL_UNSIGNED_SHORT, nullptr);
        debug::gl::test();
    }
}
