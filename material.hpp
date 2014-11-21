
#ifndef __material__included__
#define __material__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "gpu_program.hpp"
#include "texture.hpp"
#include "SFML/Graphics.hpp"

#include <glbinding/gl/gl.h>
#include <map>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename component_t>
    class color_rgb
    {
        component_t _r = component_t();
        component_t _g = component_t();
        component_t _b = component_t();

    public:
        property_get (R, _r)
        property_get (G, _g)
        property_get (B, _b)


    public:
        color_rgb (component_t r, component_t g, component_t b) : _r (r), _g (g), _b(b)
        { }

        color_rgb()
        { }

        color_rgb (const sf::Color &sfColor) : _r (component_t (sfColor.r) / 255),
                                               _g (component_t (sfColor.g) / 255),
                                               _b (component_t (sfColor.b) / 255)
        { }

        color_rgb (component_t grayscale) : _r (grayscale), _g (grayscale), _b (grayscale)
        { }

        color_rgb (math3D::vector3<component_t> colorAsVector) : _r (colorAsVector.getX()),
                                                                 _g (colorAsVector.getY()),
                                                                 _b (colorAsVector.getZ())
        { }

        math3D::vector3<component_t> asVector() const
        {
            return *((math3D::vector3<component_t>*) (this));
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    class graphics_renderer;

//----------------------------------------------------------------------------------------------------------------------

    class technique
    {
    protected:
        gpu_program::ptr  _renderingProgram;
        bool _withoutTransform = false;

    public:
        property_get (RenderingProgram, _renderingProgram)


    public:
        declare_ptr_alloc (technique)

        technique (gpu_program::ptr renderingProgram) : _renderingProgram (renderingProgram)
        { }

        void setup (graphics_renderer &renderer) const;

        void transformNotNeeded();
    };

//----------------------------------------------------------------------------------------------------------------------

    class material
    {
    protected:
        technique::ptr  _technique;
        std::map<string, texture::ptr>  _textures;
        std::map<string, float> _scalarParams;
        std::map<string, math3D::vector3_f>  _vec3Params;

    public:
        property_get (Technique, _technique)
        property_ref (textures, _textures)
        property_ref (scalarParams, _scalarParams)
        property_ref (vec3Params, _vec3Params)


    public:
        declare_ptr_alloc (material)
        material (technique::ptr renderingTechnique) : _technique (renderingTechnique)
        { }

        void setup (graphics_renderer &renderer) const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
