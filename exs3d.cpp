
#include "exs3d.hpp"
#include "mesh_impl.hpp"
#include "resource_manager_impl.hpp"
#include "render_resources.hpp"
#include "binary_stream_impl.hpp"

#include "3rd-party/forsythtriangleorderoptimizer.h"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    exs3d_mesh::exs3d_mesh (const std::string &fileName, resources &renderResources) :
        _mesh (_loadMeshFromFile (fileName, renderResources))
    {
        resource::_loaded();
    }


    const string& exs3d_mesh::_emptyLine() const
    {
        static string defaultStr = "";
        return defaultStr;
    }


    const string& exs3d_mesh::_nextLineInFile (std::ifstream &infile) const
    {
        static string nextLine;
        nextLine.reserve (100);

        while (true)
        {
            if (infile) std::getline (infile, nextLine);
            else return _emptyLine();

            if (!nextLine.empty() && nextLine.find_first_not_of (' ') != std::string::npos) return nextLine;
        }
    }


    void exs3d_mesh::_loadVertices (std::ifstream &infile, vector<exs3d_vertex> &vertices, unsigned totalCount) const
    {
        vertices.reserve (totalCount);

        auto nextLine = _emptyLine();
        while (!(nextLine = _nextLineInFile (infile)).empty())
        {
            std::istringstream parser (nextLine);
            float cx, cy, cz, nx, ny, nz, u, v;
            parser >> cx >> cy >> cz >> nx >> ny >> nz >> u >> v;

            exs3d_vertex nextVertex;
            nextVertex.coords = vector3_f (cx, cy, cz);
            nextVertex.normal = vector3_f (nx, ny, nz);
            nextVertex.tex    = vector2_f (u, v);
            vertices.push_back (nextVertex);

            if (vertices.size() == totalCount) break;
        }
    }


    void exs3d_mesh::_loadIndices (std::ifstream &infile, vector<unsigned short> &indices, unsigned totalCount) const
    {
        indices.reserve (totalCount);

        auto nextLine = _emptyLine();
        while (!(nextLine = _nextLineInFile (infile)).empty())
        {
            std::istringstream parser (nextLine);
            unsigned short i1, i2, i3;
            parser >> i1 >> i2 >> i3;

            indices.push_back (i1);
            indices.push_back (i2);
            indices.push_back (i3);

            if (indices.size() == totalCount) break;
        }
    }


    exs3d_mesh::mesh_component_t* exs3d_mesh::
    _loadMeshComponent (std::ifstream &infile, resources &renderResources) const
    {
        vector<exs3d_mesh::exs3d_vertex> vertices;
        vector<unsigned short> indices;
        texture::ptr componentTexture;
        string componentName;

        auto nextLine = _nextLineInFile (infile);
        if (nextLine.empty()) return nullptr;
        if (nextLine.find ("component") != 0)
        {
            debug::log::println_err ("error loading exs3d; expecting component defenition");
            return nullptr;
        }
        else
        {
            std::istringstream parser (nextLine);
            parser >> componentName >> componentName;
        }

        debug::log::println (mkstr ("loading component '", componentName, "'"));

        while (!(nextLine = _nextLineInFile (infile)).empty())
        {
            if (nextLine.find ("texture") == 0)
            {
                std::istringstream parser (nextLine);
                string textureName;
                parser >> textureName >> textureName;

                textureName = mkstr ("/home/leonid/Загрузки/3d1/", textureName);
                componentTexture = renderResources.texturesManager().request (textureName);

                continue;
            }

            const string verticesKeyword = "vertices";
            if (nextLine.find (verticesKeyword) == 0)
            {
                unsigned totalVerticesCount =
                        (unsigned) std::atoi (nextLine.substr (verticesKeyword.size()+1,
                                                    nextLine.find (']', verticesKeyword.size()+1)).c_str());
                _loadVertices (infile, vertices, totalVerticesCount);

                //if (indices.size() != 0) break;
                continue;
            }

            const string polygonsKeyword = "polygons";
            if (nextLine.find (polygonsKeyword) == 0)
            {
                unsigned totalIndicesCount =
                        (unsigned) std::atoi (nextLine.substr (polygonsKeyword.size()+1,
                                                               nextLine.find (']', polygonsKeyword.size()+1)).c_str());
                _loadIndices (infile, indices, totalIndicesCount);

                //if (vertices.size() != 0) break;
                break;
            }

            debug::log::println_err ("error loading exs3d; expecting vertices or faces");
            return nullptr;
        }

        //vector<unsigned short> indicesCopy (indices.size());
        //Forsyth::OptimizeFaces (indices.data(), indices.size(), vertices.size(), indicesCopy.data(), 20);

        auto shader = renderResources.gpuProgramsManager().request (gpu_program::id (exs3d_vertex_layout::alloc(),
                                                                                        "/home/leonid/Dev/glGraphics/shader.vert", "/home/leonid/Dev/glGraphics/shader.frag"));
        //auto shader = gpu_program::alloc (exs3d_vertex_layout::alloc(), "shader.vert", "shader.frag");
        if (componentTexture) componentTexture->filtering (texture::linear_MipmapLinear);
        auto mat = material::alloc (technique::alloc (shader));
        mat->textures()["uTexture"] = componentTexture;

        debug::log::println (mkstr ("'", componentName, "' loaded ", vertices.size(), " vertices;  ", indices.size() / 3, " faces"));
        return new mesh_component_t (mat, vertices, indices, componentName);
    }


    mesh * exs3d_mesh::_loadMeshFromFile (const std::string& fileName, resources &renderResources) const
    {
        debug::log::println (mkstr ("started loading exs3d from '", fileName, "'"));

        std::ifstream infile (fileName);
        if (!infile)
        {
            debug::log::println_err (mkstr ("failed opening exs3d file: ", fileName));
            return nullptr;
        }

        auto nextLine = _nextLineInFile (infile);
        if (nextLine.find ("exs3d text") != 0)
        {
            debug::log::println_err ("error loading exs3d; expecting header");
            return nullptr;
        }

        mesh *mesh_ = new mesh;
        while (infile)
        {
            auto ptr = _loadMeshComponent (infile, renderResources);
            if (ptr == nullptr) break;
            auto nextComponent = shared_ptr<mesh_component_t> (ptr);
            mesh_->addComponent (nextComponent);
        }

        return mesh_;
    }


//----------------------------------------------------------------------------------------------------------------------


    mesh::ptr exs3d_loader::loadMesh (const string &fileName, resources &otherResources)
    {
        try { return _loadMesh (fileName, otherResources); }
        catch (const loading_exception &loadingException)
        {
            debug::log::println_err (mkstr ("error loading exs3d from '", fileName, "': ", loadingException.what()));
        }

        _inputFileReader = unique_ptr<utils::binary_reader>();
        return mesh::ptr();
    }


    mesh::ptr exs3d_loader::_loadMesh (const string &fileName, resources &otherResources)
    {
        debug::log::println (mkstr ("starting loading exs3d mesh from '", fileName, "'"));
        if (!boost::filesystem::exists (fileName))  throw loading_exception ("the file doesn't exist");

        _inputFile = std::make_shared<std::ifstream> (fileName, std::ios_base::binary);
        _inputFileReader = unique_ptr<utils::binary_reader> (new utils::binary_reader (_inputFile));

        _checkHeader();
        if (_checkBinary()) return _loadBinary (otherResources);

        return mesh::ptr();  // TODO: parse text version also
    }


    void exs3d_loader::_checkHeader()
    {
        const size_t headerSize = 5;
        string header = _inputFileReader->readString (headerSize);
        if (header != "exs3d")  throw loading_exception ("invalid or corrupted file");
    }


    bool exs3d_loader::_checkBinary()
    {
        const size_t blockSize = 3;
        string typeBlock = _inputFileReader->readString (blockSize);
        return typeBlock == "bin";
    }


    mesh::ptr exs3d_loader::_loadBinary (resources &otherResources)
    {
        debug::log::println ("loading exs3d as binary");
        auto mesh = mesh::alloc();

        while (!_inputFile->eof())
            mesh->addComponent (_loadComponentBinary (otherResources));

        return mesh;
    }


    exs3d_loader::mesh_component_t::ptr exs3d_loader::_loadComponentBinary (resources &otherResources)
    {
        string componentName = _inputFileReader->readShortString();
        debug::log::println (mkstr ("loading component '", componentName, "'"));
        string textureName = _inputFileReader->readShortString();

        texture::ptr componentTexture;
        if (textureName.length())
        {
            textureName = mkstr ("/home/leonid/Загрузки/3d1/", textureName);
            componentTexture = otherResources.texturesManager().acquireFromFile (textureName);
        }

        vector<exs3d_vertex> vertices = _inputFileReader->readArrayOf<exs3d_vertex>();
        vector<unsigned short> indices = _inputFileReader->readArrayOf<unsigned short>();

        auto shader = otherResources.gpuProgramsManager().request (gpu_program::id (exs3d_vertex_layout::alloc(),
                                                                                        "/home/leonid/Dev/glGraphics/shader.vert", "/home/leonid/Dev/glGraphics/shader.frag"));
        if (componentTexture)  componentTexture->filtering (texture::linear_MipmapLinear);
        auto mat = material::alloc (technique::alloc (shader));
        mat->textures()["uTexture"] = componentTexture;

        debug::log::println (mkstr ("'", componentName, "' loaded ", vertices.size(), " vertices;  ", indices.size() / 3, " faces"));
        return mesh_component_t::alloc (mat, vertices, indices, componentName);
    }
}
