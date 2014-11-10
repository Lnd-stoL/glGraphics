
#include "exs3d.hpp"
#include "mesh_impl.hpp"
#include "resource_manager_impl.hpp"
#include "render_resources.hpp"
#include "binary_stream_impl.hpp"

#include <boost/filesystem.hpp>

#include "3rd-party/forsythtriangleorderoptimizer.h"

using oo_extensions::mkstr;
namespace fs = boost::filesystem;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    exs3d_mesh::exs3d_mesh (const std::string &fileName, resources &renderResources)
    {
        exs3d_loader loader;
        _mesh = loader.loadMesh (fileName, renderResources);

        resource::_loaded();
    }

//----------------------------------------------------------------------------------------------------------------------

    mesh::ptr exs3d_loader::loadMesh (const string &fileName, resources &otherResources)
    {
        mesh::ptr loadedMesh;
        try { loadedMesh = _loadMesh (fileName, otherResources); }
        catch (const loading_exception &loadingException)
        {
            debug::log::println_err (mkstr ("error loading exs3d from '", fileName, "': ", loadingException.what()));
        }
        catch (...)
        {
            int a;
        }

        if (_inputFile)  _inputFile->close();
        _inputFile = shared_ptr<std::ifstream>();
        _inputFileName.clear();

        return loadedMesh;
    }


    mesh::ptr exs3d_loader::_loadMesh (const string &fileName, resources &otherResources)
    {
        debug::log::println (mkstr ("starting loading exs3d mesh from '", fileName, "'"));
        if (!boost::filesystem::exists (fileName))  throw loading_exception ("the file doesn't exist");

        _inputFileName = fileName;
        if (_inputFile)  _inputFile->close();
        _inputFile = std::make_shared<std::ifstream> (fileName, std::ios_base::binary);
        _inputFileReader = unique_ptr<utils::binary_reader> (new utils::binary_reader (_inputFile));

        _fillAdditionalSearchLocations();
        _checkHeader();

        mesh::ptr loadedMesh;
        if (_checkBinary()) loadedMesh = _loadBinary (otherResources);
        else                loadedMesh = _loadText   (otherResources);

        return loadedMesh;
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
        debug::log::println ("assuming internal format is binary");
        auto mesh = mesh::alloc();

        while (!_inputFile->eof())
            mesh->addComponent (_loadComponentBinary (otherResources));

        return mesh;
    }


    exs3d_mesh::mesh_component_t::ptr exs3d_loader::_loadComponentBinary (resources &otherResources)
    {
        string componentName = _inputFileReader->readShortString();
        debug::log::println (mkstr ("loading component '", componentName, "'"));
        string textureName = _inputFileReader->readShortString();

        vector<exs3d_mesh::vertex> vertices = _inputFileReader->readArrayOf<exs3d_mesh::vertex>();
        vector<unsigned short> indices = _inputFileReader->readArrayOf<unsigned short>();

        return _constructComponent (otherResources, componentName, vertices, indices, textureName);
    }

//----------------------------------------------------------------------------------------------------------------------

    const string& exs3d_loader::_emptyLine() const
    {
        static string defaultStr = "";
        return defaultStr;
    }


    const string& exs3d_loader::_nextLineInFile() const
    {
        static string nextLine;
        nextLine.reserve (100);

        while (true)
        {
            if (*_inputFile) std::getline (*_inputFile, nextLine);
            else return _emptyLine();

            if (!nextLine.empty() && nextLine.find_first_not_of (' ') != std::string::npos) return nextLine;
        }
    }


    void exs3d_loader::_loadVertices_Text (vector<exs3d_mesh::vertex> &vertices, unsigned totalCount) const
    {
        vertices.reserve (totalCount);

        auto nextLine = _emptyLine();
        while (!(nextLine = _nextLineInFile()).empty())
        {
            std::istringstream parser (nextLine);
            float cx = 0, cy = 0, cz = 0, nx = 0, ny = 0, nz = 0, u = 0, v = 0;
            parser >> cx >> cy >> cz >> nx >> ny >> nz >> u >> v;

            exs3d_mesh::vertex nextVert = {{cx, cy, cz}, {nx, ny, nz}, {u, v}};
            vertices.emplace_back (std::move (nextVert));
            if (vertices.size() == totalCount) break;
        }
    }


    void exs3d_loader::_loadIndices_Text (vector<unsigned short> &indices, unsigned totalCount) const
    {
        indices.reserve (totalCount);

        auto nextLine = _emptyLine();
        while (!(nextLine = _nextLineInFile()).empty())
        {
            std::istringstream parser (nextLine);
            unsigned short i1 = 0, i2 = 0, i3 = 0;
            parser >> i1 >> i2 >> i3;

            indices.push_back (i1);
            indices.push_back (i2);
            indices.push_back (i3);

            if (indices.size() == totalCount) break;
        }
    }


    mesh::ptr exs3d_loader::_loadText (resources &otherResources)
    {
        debug::log::println ("assuming internal format is textual");

        _inputFile->close();
        _inputFile = std::make_shared<std::ifstream> (_inputFileName);

        auto nextLine = _nextLineInFile();
        if (nextLine.find ("exs3d text") != 0)
            throw loading_exception ("invalid header");

        mesh::ptr loadedMesh = mesh::alloc();
        while (!_inputFile->eof())
        {
            auto nextComponent = _loadComponentText (otherResources);
            if (nextComponent) loadedMesh->addComponent (nextComponent);
        }

        return loadedMesh;
    }


    exs3d_mesh::mesh_component_t::ptr exs3d_loader::_loadComponentText (resources &otherResources)
    {
        vector<exs3d_mesh::vertex> vertices;
        vector<unsigned short> indices;
        string textureName;
        string componentName;

        auto nextLine = _nextLineInFile();
        if (nextLine.empty())
            return exs3d_mesh::mesh_component_t::ptr();

        if (nextLine.find ("component") != 0)
            throw loading_exception ("error loading exs3d; expecting component defenition");
        else
        {
            std::istringstream parser (nextLine);
            parser >> componentName >> componentName;
        }

        debug::log::println (mkstr ("loading component '", componentName, "'"));

        while (!(nextLine = _nextLineInFile()).empty())
        {
            if (nextLine.find ("texture") == 0)
            {
                std::istringstream parser (nextLine);
                parser >> textureName >> textureName;

                continue;
            }

            const string verticesKeyword = "vertices";
            if (nextLine.find (verticesKeyword) == 0)
            {
                unsigned totalVerticesCount =
                        (unsigned) std::atoi (nextLine.substr (verticesKeyword.size()+1,
                                                               nextLine.find (']', verticesKeyword.size()+1)).c_str());
                _loadVertices_Text (vertices, totalVerticesCount);

                continue;
            }

            const string polygonsKeyword = "polygons";
            if (nextLine.find (polygonsKeyword) == 0)
            {
                unsigned totalIndicesCount =
                        (unsigned) std::atoi (nextLine.substr (polygonsKeyword.size()+1,
                                                               nextLine.find (']', polygonsKeyword.size()+1)).c_str());
                _loadIndices_Text (indices, totalIndicesCount);

                break;
            }

            throw loading_exception ("error loading exs3d; expecting vertices or faces");
        }

        return _constructComponent (otherResources, componentName, vertices, indices, textureName);
    }


    exs3d_mesh::mesh_component_t::ptr
    exs3d_loader::_constructComponent (resources &otherResources, string name, vector<exs3d_mesh::vertex> &vertices,
                                        vector<unsigned short> &indices, string textureName)
    {
        auto vertexLayout = exs3d_mesh::exs3d_vertex_layout::alloc();
        auto shaderResourceId = gpu_program::id (vertexLayout, "shader.vert", "shader.frag");
        auto shader = otherResources.gpuProgramsManager().request (shaderResourceId, otherResources);
        auto mat = material::alloc (technique::alloc (shader));

        if (!textureName.empty())
        {
            auto txt = otherResources.texturesManager().requestWithAdditionalLoactions (textureName,
                                                                                        _additionalSearchLocations);
            txt->filtering (texture::linear_MipmapLinear);
            mat->textures()["uTexture"] = txt;
        }

        debug::log::println (mkstr ("component '", name, "' loaded ", vertices.size(), " vertices;  ", indices.size() / 3, " faces"));
        return exs3d_mesh::mesh_component_t::alloc (mat, vertices, indices, name);
    }


    void exs3d_loader::_fillAdditionalSearchLocations()
    {
        fs::path pathToInputFile (_inputFileName);
        pathToInputFile.remove_filename();

        _additionalSearchLocations.clear();
        _additionalSearchLocations.resize (2);
        _additionalSearchLocations[0] = pathToInputFile.string();
        _additionalSearchLocations[1] = (pathToInputFile / "textures").string();
    }
}
