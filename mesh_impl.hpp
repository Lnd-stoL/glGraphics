
#include "mesh.hpp"
#include "gpu_buffer_impl.hpp"
#include "scene.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename vertex_t, typename index_t>
    void mesh_component<vertex_t, index_t>::draw (graphics_renderer &renderer) const
    {
        renderer.use (_material);
        renderer.draw (*_vertexBuffer, *_indexBuffer, _indexBuffer->bytesPerIndex());
    }
}
