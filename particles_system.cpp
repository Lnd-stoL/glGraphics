
#include "particles_system.hpp"

#include "gpu_buffer_impl.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void particle_system_object::draw (graphics_renderer &renderer) const
    {
        transformable_renderable_object::_setupObject2ScreenTransform (renderer);

        matrix_4x4_f mvpMatrix = renderer.state().getObject2ScreenTransform().getWorldTransform().asMatrix().convertType<float>();
        vector3_f baseUp    = mvpMatrix.col3 (1);
        vector3_f baseRight = mvpMatrix.col3 (0);

        _material->vec3Params()["uBaseUp"]    = baseUp.normalized();
        _material->vec3Params()["uBaseRight"] = baseRight.normalized();

        particle_vertex *vertices = _vertexBuffer->cpuAccess (gpu_buffer::accessReadWrite);
        _modifyParticles (vertices);
        _sortParticles (vertices, renderer);
        _vertexBuffer->gpuAccess();

        renderer.use (_material);
        renderer.blend (true);
        //renderer.testDepth (false);
        renderer.drawPoints (*_vertexBuffer);
        renderer.blend (false);
        //renderer.testDepth (true);
    }


    particle_system_object::particle_system_object (const transform_d &transform, resources &res)
        : transformable_renderable_object (transform)
    {
        auto particleShaderId = gpu_program::id (particle_vertex_layout::alloc(),
                                                  "particle.vert", "particle.geom", "particle.frag");
        auto particleShader = res.gpuProgramsManager().request (particleShaderId, res);
        _material = material::alloc (technique::alloc (particleShader));
        _material->textures()["uTexture"] = res.requestFromFile<texture> ("SmokeShape.png");

        _generateVertexBuffer();
    }


    billboard_particle::billboard_particle (const vector2_d &size) : _size (size)
    {

    }


    void particle_system_object::_generateVertexBuffer()
    {
        unsigned particlesNum = 30;
        vector<particle_vertex>  vertices (particlesNum);

        for (unsigned i = 0; i < particlesNum; ++i)
        {
            float rnd1 = float (std::rand() % 1000) / 500 - 1;
            float rnd2 = float (std::rand() % 1000) / 500 - 1;
            float rnd3 = float (std::rand() % 1000) / 500 - 1;

            vertices[i].position = vector3_f (rnd1 * 2, rnd3 * 1, rnd2 * 2);
            vertices[i].size = vector2_f (3, 3);
        }

        _vertexBuffer = vertex_buffer<particle_vertex>::alloc (vertices);
    }


    void particle_system_object::_modifyParticles (particle_vertex *vertices)  const
    {
        for (unsigned i = 0; i < _vertexBuffer->getSize(); ++i)
        {
            float rnd1 = float (std::rand() % 1000) / 500 - 1;
            float rnd2 = float (std::rand() % 1000) / 500 - 1;
            float rnd3 = float (std::rand() % 1000) / 500 - 1;

            vertices[i].position += vector3_f (rnd1, rnd3, rnd2) * 0.1;
            vertices[i].size += vector2_f (rnd1, rnd3) * 0.05;
        }
    }


    void particle_system_object::_sortParticles (particle_vertex *vertices , graphics_renderer &renderer)  const
    {
        vector3_f viewerPos = renderer.state().getCamera()->getTransform().getTranslation().convertType<float>();

        auto sortingComparer = [viewerPos] (const particle_vertex& v1, const particle_vertex &v2)
        {
            return (viewerPos - v1.position).squaredLength() >
                   (viewerPos - v2.position).squaredLength() + 0.00001f;
        };

        std::sort (vertices, vertices + _vertexBuffer->getSize(), sortingComparer);
    }

}
