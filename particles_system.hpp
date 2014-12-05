
#ifndef __particles_system_included__
#define __particles_system_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "transformable_object.hpp"
#include "gpu_buffer.hpp"
#include "material.hpp"
#include "render_resources.hpp"

using math3d::vector2_f;
using math3d::vector3_f;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class billboard_particle
    {
        vector2_d  _size;

    public:
        property_get_ref (Size, _size)


    public:
        declare_ptr_alloc (billboard_particle)
        billboard_particle (const vector2_d &size);
    };

//----------------------------------------------------------------------------------------------------------------------

    class particle_system_object :
        public transformable_renderable_object
    {
    public:
        struct particle_vertex
        {
            vector2_f  size;
            vector3_f  position;
        };


        struct particle_vertex_layout : vertex_layout<particle_vertex>
        {
        protected:
            virtual void _registerAttributes()
            {
                _registerAttribute ("aSize", attribute::tFloat, offsetof (particle_vertex, size), 2);
                _registerAttribute ("aPosition", attribute::tFloat, offsetof (particle_vertex, position), 3);
            }

        public:
            declare_ptr_alloc (particle_vertex_layout)
            particle_vertex_layout()  { _registerAttributes(); }
        };


    private:
        material::ptr  _material;
        vertex_buffer<particle_vertex>::ptr  _vertexBuffer;


    protected:
        void _generateVertexBuffer();
        void _modifyParticles (particle_vertex *vertices)  const;
        void _sortParticles (particle_vertex *vertices, graphics_renderer &renderer)  const;

    public:
        declare_ptr_alloc (particle_system_object)
        particle_system_object (const transform_d &transform, resources &res);

        virtual  void draw (graphics_renderer &renderer)  const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
