
#ifndef __material__included__
#define __material__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "gpu_program.hpp"
#include "vertex_buffer.hpp"

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class material
    {
    protected:
        shared_ptr<gpu_program> _renderingProgram;

    public:
        property_get (RenderingProgram, _renderingProgram)

    public:
        material (shared_ptr<gpu_program> renderingProgram) : _renderingProgram (renderingProgram)
        { }

        virtual void use() const;
    };

    //------------------------------------------------------------------------------------------------------------------

    class textured_material : public material
    {
    protected:
        shared_ptr<sf::Texture> _texture;

    public:
        property_get (Texture, _texture)

    public:
        textured_material (shared_ptr<gpu_program> renderingProgram, shared_ptr<sf::Texture> texture) :
                material (renderingProgram),
                _texture (texture)
        { }

        virtual void use() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
