
#include "particles_system.hpp"

#include "gpu_buffer_impl.hpp"
#include "resource_manager_impl.hpp"
#include "graphics_renderer.hpp"

using oo_extensions::mkstr;
using math3d::matrix_4x4_f;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void particle_system_object::draw (graphics_renderer &renderer) const
    {
        transformable_renderable_object::_setupObject2ScreenTransform (renderer);

        matrix_4x4_f mvpMatrix = renderer.state().object2ScreenTransform().worldTransform().asMatrix().convertType<float>();
        vector3_f baseUp    = mvpMatrix.col3 (1);
        vector3_f baseRight = mvpMatrix.col3 (0);

        _material->set ("uBaseUp", baseUp.normalized());
        _material->set ("uBaseRight", baseRight.normalized());

        particle_vertex *vertices = _vertexBuffer->cpuAccess (gpu_buffer::accessReadWrite);
        _modifyParticles (vertices);
        _sortParticles (vertices, renderer);
        _vertexBuffer->gpuAccess();

        renderer.use (_material);
        renderer.blend (true);
        //renderer.testDepth (false);
        renderer.drawPoints (*_vertexBuffer);
        //renderer.testDepth (true);
    }


    particle_system_object::particle_system_object (const transform_d &transform, resources &res)
        : transformable_renderable_object (transform)
    {
        auto particleShaderId = gpu_program::id (particle_vertex_layout::alloc(),
                                                  "particles/particle.vert", "particles/particle.geom",
                                                  "particles/particle.frag");

        auto particleShader = res.gpuProgramsManager().request (particleShaderId, res);
        _material = material::alloc (technique::alloc (particleShader));
        _material->set ("uTexture", res.requestFromFile<texture> ("SmokeShape.png"));

        _generateVertexBuffer();
    }


    billboard_particle::billboard_particle (const vector2_d &size) : _size (size)
    {

    }


    void particle_system_object::_generateVertexBuffer()
    {
        unsigned particlesNum = 1500;
        vector<particle_vertex>  vertices (particlesNum);

        for (unsigned i = 0; i < particlesNum; ++i)
        {
            float rnd1 = float (std::rand() % 1000) / 500 - 1;
            float rnd2 = float (std::rand() % 1000) / 500 - 1;
            float rnd3 = float (std::rand() % 1000) / 500 - 1;

            vertices[i].position = vector3_f (rnd1 * 2, rnd3 * 1, rnd2 * 2);
            vertices[i].size = vector2_f (1.3, 1.3);
        }

        _vertexBuffer = vertex_buffer<particle_vertex>::alloc (vertices);
    }


    void particle_system_object::_modifyParticles (particle_vertex *vertices)  const
    {
        for (unsigned i = 0; i < _vertexBuffer->size(); ++i)
        {
            float rnd1 = float (std::rand() % 1000) / 500 - 1;
            float rnd2 = float (std::rand() % 1000) / 500 - 1;
            float rnd3 = float (std::rand() % 1000) / 500 - 1;

            vertices[i].position += vector3_f (rnd1, rnd3, rnd2) * 0.12;
            vertices[i].size += vector2_f (rnd1, rnd3) * 0.05;
        }
    }


    void particle_system_object::_sortParticles (particle_vertex *vertices , graphics_renderer &renderer)  const
    {
        vector3_f viewerPos = renderer.state().activeCamera()->transform().ttranslation().convertType<float>();

        auto sortingComparer = [viewerPos] (const particle_vertex& v1, const particle_vertex &v2)
        {
            return (viewerPos - v1.position).squaredLength() >
                   (viewerPos - v2.position).squaredLength() + 0.00001f;
        };

        std::sort (vertices, vertices + _vertexBuffer->size(), sortingComparer);
    }

}
