
#ifndef __mesh__included__
#define __mesh__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "gpu_buffer.hpp"
#include "material.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class basic_mesh_component
    {
    protected:
        string _name;
        material::ptr _material;

    public:
        property_get (Material, _material)
        property_get_ref (Name, _name)


    public:
        declare_ptr_alloc (basic_mesh_component)

        basic_mesh_component (material::ptr mat, const string &name) : _material (mat), _name (name)
        { }

        virtual ~basic_mesh_component() { }

        virtual void draw() const = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t, typename index_t>
    class mesh_component :
        public basic_mesh_component,
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
                        string name = "") : basic_mesh_component (material, name),
                                            _vertexBuffer (vertexBuffer),
                                            _indexBuffer  (indexBuffer)
        { }


        mesh_component (material::ptr material,
                        const vector<vertex_t> &vertices,
                        const vector<index_t>  &indices,
                        string name = "") : basic_mesh_component (material, name),
                                            _vertexBuffer (make_shared<vertex_buffer<vertex_t>> (vertices)),
                                            _indexBuffer  (make_shared<index_buffer<index_t>> (indices))
        { }

        void draw() const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class static_mesh :
        public oo_extensions::non_copyable
    {
    protected:
        vector<basic_mesh_component::ptr> _components;

    public:
        property_get_ref (Components, _components)


    public:
        declare_ptr_alloc (static_mesh)
        static_mesh() { };

        void addComponent (basic_mesh_component::ptr component);
        void removeComponent (const string &name);

        void draw() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
