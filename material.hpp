
#ifndef __material__included__
#define __material__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "gpu_program.hpp"
#include "gpu_buffer.hpp"
#include "camera.hpp"
#include "texture.hpp"

#include <GL/glew.h>
#include <vector>
#include <memory>

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

        color_rgb (component_t grayscale) : _r (grayscale), _g (grayscale), _b (grayscale)
        { }

        color_rgb (math3D::vector3<component_t> colorAsVector) : _r (colorAsVector.getX()),
                                                                 _g (colorAsVector.getY()),
                                                                 _b (colorAsVector.getZ())
        { }

    };

//----------------------------------------------------------------------------------------------------------------------

    class material
    {
    protected:
        gpu_program::ptr _renderingProgram;

    public:
        property_get (RenderingProgram, _renderingProgram)

    public:
        declare_ptr_alloc (material)
        material (gpu_program::ptr renderingProgram) : _renderingProgram (renderingProgram)
        { }

        virtual void use() const;
        virtual void setupViewerTransform (const math3D::object2screen_transform_f &transform);
    };

//----------------------------------------------------------------------------------------------------------------------

    class textured_material : public material
    {
    protected:
        texture::ptr _texture;

    public:
        property_get (Texture, _texture)

    public:
        declare_ptr_alloc (textured_material)

        textured_material (gpu_program::ptr renderingProgram, texture::ptr texture) :
                material (renderingProgram),
                _texture (texture)
        {
            _renderingProgram->setUniformSampler ("uTexture", 0);
        }

        void changeTexture (texture::ptr texture)
        {
            _texture = texture;
        }

        virtual void use() const;
        virtual void setupViewerTransform (const math3D::object2screen_transform_f &transform);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
