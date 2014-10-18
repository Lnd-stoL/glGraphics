
#ifndef __exs3d__included__
#define __exs3d__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "resource.hpp"

#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>

using math3D::vector3_f;
using math3D::vector2_f;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class resources;

    class exs3d_mesh : public resource
    {
    public:
        struct exs3d_vertex
        {
            vector3_f coords;
            vector3_f normal;
            vector2_f tex;
        };


        struct exs3d_vertex_layout : vertex_layout<exs3d_vertex>
        {
        protected:
            virtual void _registerAttributes()
            {
                _registerAttribute ("aCoords", attribute::tFloat, offsetof (exs3d_vertex, coords), 3);
                _registerAttribute ("aNormal", attribute::tFloat, offsetof (exs3d_vertex, normal), 3, true);
                _registerAttribute ("aTexUV",  attribute::tFloat, offsetof (exs3d_vertex, tex), 2);
            }

        public:
            declare_ptr_alloc (exs3d_vertex_layout)
            exs3d_vertex_layout()  { _registerAttributes(); }
        };


    public:
        typedef mesh_component<exs3d_vertex, unsigned short> mesh_component_t;


    protected:
        static_mesh::ptr _mesh;

    protected:
        static_mesh* _loadMeshFromFile (const std::string &fileName, resources &res) const;
        mesh_component_t* _loadMeshComponent (std::ifstream &infile, resources &res) const;
        const string& _nextLineInFile (std::ifstream &infile) const;
        void _loadVertices (std::ifstream &infile, vector<exs3d_vertex> &vertices, unsigned totalCount) const;
        void _loadIndices (std::ifstream &infile, vector<unsigned short> &index, unsigned totalCount) const;
        const string& _emptyLine() const;


    public:
        property_get (Mesh, _mesh)


    public:
        declare_ptr_alloc (exs3d_mesh)
        exs3d_mesh (const std::string &fileName, resources &renderResources);
        void draw (const camera &viewer) const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
