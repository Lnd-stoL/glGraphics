
#ifndef __mesh__included__
#define __mesh__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "gpu_buffer.hpp"
#include "material.hpp"
#include "renderable.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class a_mesh_component : public renderable
    {
    protected:
        string        _name;
        material::ptr _material;

    public:
        property_get (Material, _material)
        property_get_ref (Name, _name)


    public:
        declare_ptr_alloc (a_mesh_component)

        a_mesh_component (material::ptr mat, const string &name = "<unnamed>") : _material (mat), _name (name)
        { }

        virtual ~a_mesh_component () { }
        virtual void draw (graphics_renderer &renderer) const = 0;

        void changeMaterial (material::ptr newMaterial);
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t, typename index_t>
    class mesh_component :
        public a_mesh_component,
        public oo_extensions::non_copyable
    {
        typename vertex_buffer<vertex_t>::ptr _vertexBuffer;
        typename index_buffer<index_t>::ptr   _indexBuffer;

    public:
        typedef mesh_component<vertex_t, index_t> this_t;
        declare_ptr_alloc (this_t)

        mesh_component (material::ptr material,
                        typename vertex_buffer<vertex_t>::ptr vertexBuffer,
                        typename index_buffer<index_t>::ptr indexBuffer,
                        string name = "") : a_mesh_component (material, name),
                                            _vertexBuffer (vertexBuffer),
                                            _indexBuffer  (indexBuffer)
        { }


        mesh_component (material::ptr material,
                        const vector<vertex_t> &vertices,
                        const vector<index_t>  &indices,
                        string name = "") : a_mesh_component (material, name),
                                            _vertexBuffer (vertex_buffer<vertex_t>::alloc (vertices)),
                                            _indexBuffer  (index_buffer<index_t>::alloc (indices))
        { }

        virtual void draw (graphics_renderer &renderer) const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class mesh :
        public oo_extensions::non_copyable
    {
    protected:
        vector<a_mesh_component::ptr> _components;

    public:
        property_get_ref (Components, _components)


    public:
        declare_ptr_alloc (mesh)
        mesh() { };

        void addComponent (a_mesh_component::ptr component);
        void removeComponent (const string &name);

        virtual void draw (graphics_renderer &renderer) const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class elementary_shapes
    {
    public:
        struct simple_vertex
        {
            math3D::vector3_f coords;
            math3D::vector2_f texUV;
        };

        struct simple_vertex_layout : vertex_layout<simple_vertex>
        {
        protected:
            virtual void _registerAttributes()
            {
                _registerAttribute ("aCoords", attribute::tFloat, offsetof (simple_vertex, coords), 3);
                _registerAttribute ("aTexUV",  attribute::tFloat, offsetof (simple_vertex, texUV), 2);
            }

        public:
            declare_ptr_alloc (simple_vertex_layout)
            simple_vertex_layout()  { _registerAttributes(); }
        };


    public:
        static void quadXZ (vector<simple_vertex> &vertices, vector<unsigned short> &indices, float scale = 1, float y = 0);
        static void quadXY (vector<simple_vertex> &vertices, vector<unsigned short> &indices, float scale = 1, float z = 0);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
