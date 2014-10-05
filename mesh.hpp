
#ifndef __mesh__included__
#define __mesh__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "vertex_buffer.hpp"
#include "material.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class basic_mesh_component
    {
    protected:
        string _name;
        shared_ptr<material> _material;

    public:
        property_get (Material, _material)
        property_get_ref (Name, _name)


    public:
        basic_mesh_component (shared_ptr<material> mat, const string &name) : _material (mat), _name (name)
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
        shared_ptr<vertex_buffer<vertex_t>> _vertexBuffer;
        shared_ptr<index_buffer<index_t>>   _indexBuffer;

    public:
        mesh_component (shared_ptr<material> material,
                        shared_ptr<vertex_buffer<vertex_t>> vertexBuffer,
                        shared_ptr<index_buffer<index_t>> indexBuffer,
                        string name = "") : basic_mesh_component (material, name),
                                            _vertexBuffer (vertexBuffer),
                                            _indexBuffer  (indexBuffer)
        { }


        mesh_component (shared_ptr<material> material,
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
        vector<shared_ptr<basic_mesh_component>> _components;

    public:
        property_get_ref (Components, _components)


    public:
        static_mesh() { };

        void addComponent (shared_ptr<basic_mesh_component> component);
        void removeComponent (const string &name);

        void draw() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#include "mesh.cpp"

#endif
