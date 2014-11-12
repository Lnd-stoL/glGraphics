
#include "material.hpp"
#include "scene.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void technique::setup (graphics_renderer &renderer) const
    {
        math3D::object2screen_transform_d trans = renderer.state().getObject2ScreenTransform();

        _renderingProgram->use();

        _renderingProgram->setUniform ("uMatWorldTransform", trans.getWorldTransform().asMatrix().convertType<float>(), true);
        _renderingProgram->setUniform ("uViewPos", trans.getCameraTransform().getTranslation().convertType<float>(), true);
        _renderingProgram->setUniform ("uMatTransform", trans.asMatrix().convertType<float>());
    }


    void material::setup (graphics_renderer &renderer) const
    {
        _technique->setup (renderer);
        for (auto param : _floatingPointParams)
        {
            _technique->getRenderingProgram()->setUniform (param.first, param.second, true);
        }

        for (auto texture_ : _textures)
        {
            texture_.second->use();

            _technique->getRenderingProgram()->setUniformSampler (texture_.first, 0, true);
        }
    }
}
