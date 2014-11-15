
#include "material.hpp"
#include "scene.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void technique::transformNotNeeded()
    {
        _withoutTransform = true;
    }


    void technique::setup (graphics_renderer &renderer) const
    {
        math3D::object2screen_transform_d trans = renderer.state().getObject2ScreenTransform();

        _renderingProgram->use();

        if (!_withoutTransform)
        {
            _renderingProgram->setUniform ("uMatWorldTransform", trans.getWorldTransform().asMatrix().convertType<float>(), true);
            _renderingProgram->setUniform ("uMatTransform", trans.asMatrix().convertType<float>());
        }

        _renderingProgram->setUniform ("uViewPos", trans.getCameraTransform().getTranslation().convertType<float>(), true);
    }


    void material::setup (graphics_renderer &renderer) const
    {
        _technique->setup (renderer);
        for (auto param : _floatingPointParams)
        {
            _technique->getRenderingProgram()->setUniform (param.first, param.second, true);
        }

        unsigned i = 0;
        for (auto texture_ : _textures)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            texture_.second->use();

            _technique->getRenderingProgram()->setUniformSampler (texture_.first, i, true);
            ++i;
        }
    }
}
