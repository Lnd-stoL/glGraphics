
#include "exs3d.hpp"
#include "mesh_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    exs3d_mesh::exs3d_mesh (const std::string& fileName) : _mesh (_loadMeshFromFile (fileName))
    { }


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


    exs3d_mesh::mesh_component_t* exs3d_mesh::_loadMeshComponent (std::ifstream &infile) const
    {
        vector<exs3d_mesh::exs3d_vertex> vertices;
        vector<unsigned short> indices;
        shared_ptr<sf::Texture> texture;
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

        debug::log::println (mkstr ("Loading component '", componentName, "'"));

        while (!(nextLine = _nextLineInFile (infile)).empty())
        {
            if (nextLine.find ("texture") == 0)
            {
                std::istringstream parser (nextLine);
                string textureName;
                parser >> textureName >> textureName;

                textureName = mkstr ("/home/leonid/Загрузки/3d1/", textureName);
                texture = make_shared<sf::Texture>();
                texture->loadFromFile (textureName);
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

        auto shader = gpu_program::alloc (exs3d_vertex_layout::alloc(), "shader.vert", "shader.frag");
        auto mat = textured_material::alloc (shader, texture);

        debug::log::println (mkstr ("'", componentName, "' loaded ", vertices.size(), " vertices;  ", indices.size() / 3, " faces"));
        return new mesh_component_t (mat, vertices, indices, componentName);
    }


    static_mesh* exs3d_mesh::_loadMeshFromFile (const std::string& fileName) const
    {
        debug::log::println (mkstr ("Started loading exs3d from '", fileName, "'"));

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

        static_mesh *mesh = new static_mesh();
        while (infile)
        {
            auto ptr = _loadMeshComponent (infile);
            if (ptr == nullptr) break;
            auto nextComponent = shared_ptr<mesh_component_t> (ptr);
            mesh->addComponent (nextComponent);
        }

        return mesh;
    }


    void exs3d_mesh::draw (const camera &viewer) const
    {
        //_mesh->draw (viewer);
    }
}
