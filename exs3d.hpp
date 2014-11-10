
#ifndef __exs3d__included__
#define __exs3d__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "binary_stream.hpp"
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
//----------------------------------------------------------------------------------------------------------------------
    class resources;
//----------------------------------------------------------------------------------------------------------------------

    class exs3d_mesh : public resource
    {
    public:
        struct vertex
        {
            vector3_f coords;
            vector3_f normal;
            vector2_f texUV;
        };


        struct exs3d_vertex_layout : vertex_layout<vertex>
        {
        protected:
            virtual void _registerAttributes()
            {
                _registerAttribute ("aCoords", attribute::tFloat, offsetof (vertex, coords), 3);
                _registerAttribute ("aNormal", attribute::tFloat, offsetof (vertex, normal), 3, true);
                _registerAttribute ("aTexUV",  attribute::tFloat, offsetof (vertex, texUV), 2);
            }

        public:
            declare_ptr_alloc (exs3d_vertex_layout)
            exs3d_vertex_layout()  { _registerAttributes(); }
        };


    public:
        typedef mesh_component<vertex, unsigned short> mesh_component_t;


    protected:
        mesh::ptr _mesh;

    public:
        property_get (RenderableMesh, _mesh)


    public:
        declare_ptr_alloc (exs3d_mesh)

        exs3d_mesh (const std::string &fileName, resources &renderResources);
    };

//----------------------------------------------------------------------------------------------------------------------

    class exs3d_loader
    {
    private:
        class loading_exception : public std::exception
        {
            string _reason;

        public:
            loading_exception (const string &reason) : _reason (reason)
            { }

            virtual const char* what() const noexcept  { return _reason.c_str(); }
        };


    private:
        string                           _inputFileName   = "";
        shared_ptr<std::ifstream>        _inputFile;
        unique_ptr<utils::binary_reader> _inputFileReader;


    private:
        mesh::ptr _loadMesh (const string &fileName, resources& otherResources);
        void _checkHeader();
        bool _checkBinary();

        mesh::ptr _loadBinary (resources& otherResources);
        exs3d_mesh::mesh_component_t::ptr _loadComponentBinary (resources &otherResources);

        mesh::ptr _loadText (resources& otherResources);
        exs3d_mesh::mesh_component_t::ptr _loadComponentText (resources &otherResources);

        void _loadVertices_Text (vector<exs3d_mesh::vertex> &vertices, unsigned totalCount) const;
        void _loadIndices_Text (vector<unsigned short> &index, unsigned totalCount) const;

        const string& _nextLineInFile() const;
        const string& _emptyLine() const;


    public:
        mesh::ptr loadMesh (const string &fileName, resources& otherResources);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
