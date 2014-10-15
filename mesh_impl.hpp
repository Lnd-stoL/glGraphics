
#include "mesh.hpp"
#include "gpu_buffer_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename vertex_t, typename index_t>
    void mesh_component<vertex_t, index_t>::draw (const object2screen_transform_d &screenTransform) const
    {
        _vertexBuffer->use();
        _indexBuffer->use();

        _material->use();
        _material->setupViewerTransform (screenTransform.convertType<float>());

        glDrawElements (GL_TRIANGLES, _indexBuffer->getSize(), GL_UNSIGNED_SHORT, nullptr);
    }
}
