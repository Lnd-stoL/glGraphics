
#include "mesh.hpp"
#include "gpu_buffer_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename vertex_t, typename index_t>
    void mesh_component<vertex_t, index_t>::draw (graphics_renderer &renderer) const
    {
        _vertexBuffer->use();
        _indexBuffer->use();

        _material->setup (renderer);

        glDrawElements (GL_TRIANGLES, _indexBuffer->getSize(), GL_UNSIGNED_SHORT, nullptr);
        debug::gl::test();
    }
}
