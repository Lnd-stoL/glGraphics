
#ifndef __material__included__
#define __material__included__

//----------------------------------------------------------------------------------------------------------------------

#include "gpu_program.hpp"
#include "texture.hpp"

#include <SFML/Graphics/Color.hpp>
#include <map>
#include <unordered_map>

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
        property_get (r, _r)
        property_get (g, _g)
        property_get (b, _b)


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

        color_rgb (math3d::vector3<component_t> colorAsVector) : _r (colorAsVector.x()),
                                                                 _g (colorAsVector.y()),
                                                                 _b (colorAsVector.z())
        { }

        math3d::vector3<component_t> asVector() const
        {
            return *((math3d::vector3<component_t>*) (this));
        }

        color_rgb<component_t>& lighten (component_t amount)
        {
            _r += amount;
            _g += amount;
            _b += amount;

            return *this;
        }

        color_rgb<component_t>& contrast (component_t amount)
        {
            _r *= amount;
            _g *= amount;
            _b *= amount;

            return *this;
        }

        color_rgb<component_t>& mix (const color_rgb<component_t> &clr, component_t amount)
        {
            _r = _r * (1 - amount) + clr._r * amount;
            _g = _g * (1 - amount) + clr._g * amount;
            _b = _b * (1 - amount) + clr._b * amount;

            return *this;
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    class graphics_renderer;

//----------------------------------------------------------------------------------------------------------------------

    class technique
    {
    protected:
        gpu_program::ptr  _gpuProgram;
        bool  _withoutTransform = false;

        gpu_program::uniform_id  _mvpMatrix_UniformId        =  gpu_program::invalidUniformId;
        gpu_program::uniform_id  _worldMatrix_UniformId      =  gpu_program::invalidUniformId;
        gpu_program::uniform_id  _viewMatrix_UniformId       =  gpu_program::invalidUniformId;
        gpu_program::uniform_id  _worldViewMatrix_UniformId  =  gpu_program::invalidUniformId;

        gpu_program::uniform_id  _viewPos_UniformId = gpu_program::invalidUniformId;


    public:
        property_get (gpuProgram, _gpuProgram)


    protected:
        void _cacheUniformsLocations();

    public:
        declare_ptr_alloc (technique)
        technique (gpu_program::ptr renderingProgram);

        void setup (graphics_renderer &renderer) const;

        void transformNotNeeded();
    };

//----------------------------------------------------------------------------------------------------------------------

    class material
    {
    private:
        template<typename value_t>
        struct uniform_info
        {
            value_t  value;
            gpu_program::uniform_id  uniformId;
            bool  needsUpdate;
        };


    protected:
        technique::ptr  _technique;

        std::unordered_map<string, uniform_info<texture::ptr>>  _textures;
        std::unordered_map<string, uniform_info<float>>         _scalarParams;
        std::unordered_map<string, uniform_info<math3d::vector3_f>>  _vec3Params;
        std::unordered_map<string, uniform_info<math3d::vector2_f>>  _vec2Params;
        std::unordered_map<string, uniform_info<math3d::matrix_4x4_f>>  _mat4Params;


    public:
        property_get (renderingTechnique, _technique)


    public:
        declare_ptr_alloc (material)

        material (technique::ptr renderingTechnique) : _technique (renderingTechnique)
        { }

        void set (const string &name, texture::ptr txt);
        void set (const string &name, float param);
        void set (const string &name, const math3d::vector3_f &param);
        void set (const string &name, const math3d::vector2_f &param);
        void set (const string &name, const math3d::matrix_4x4_f &param);

        void setup (graphics_renderer &renderer) const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
