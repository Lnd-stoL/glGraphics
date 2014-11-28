
#ifndef __exs3d__included__
#define __exs3d__included__

//----------------------------------------------------------------------------------------------------------------------

#include "mesh.hpp"
#include "binary_stream.hpp"
#include "resource.hpp"

using math3d::vector3_f;
using math3d::vector2_f;

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
                _registerAttribute ("aNormal", attribute::tFloat, offsetof (vertex, normal), 3);
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

        vector<string> _additionalSearchLocations;
        technique::ptr _defaultTechnique;


    private:
        mesh::ptr _loadMesh (const string &fileName, resources& otherResources);
        void _checkHeader();
        bool _checkBinary();

        void _fillAdditionalSearchLocations();
        void _initDefaults (resources &otherResources);
        void _connectTextureToMaterial (material::ptr mat, const string& textureName, resources &otherResources);
        void _reportComponentLoaded (const a_mesh_component &meshComponent);

        mesh::ptr _loadBinary (resources& otherResources);
        material::ptr _loadMaterialBinary (resources& otherResources);
        a_mesh_component::ptr _loadComponentBinary (resources &otherResources);

        mesh::ptr _loadText (resources& otherResources);
        a_mesh_component::ptr _loadComponentText (resources &otherResources);
        material::ptr _loadMaterialText (resources& otherResources);

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
