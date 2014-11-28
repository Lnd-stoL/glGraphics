
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
        const math3d::object2screen_transform_d &trans = renderer.state().getObject2ScreenTransform();

        _renderingProgram->use();

        if (!_withoutTransform)
        {
            _renderingProgram->setUniform ("uMatWorldTransform", trans.getWorldTransformMatrix().convertType<float>(), true);
            _renderingProgram->setUniform ("uMatTransform", trans.asMatrix().convertType<float>());
            _renderingProgram->setUniform ("uMatWorldViewTransform", trans.getWorldCamTransformMatrix().convertType<float>(), true);
        }

        _renderingProgram->setUniform ("uViewPos",
                                       renderer.state().getCamera()->getTransform().getTranslation().convertType<float>(), true);
    }


    void material::setup (graphics_renderer &renderer) const
    {
        _technique->setup (renderer);
        for (auto param : _scalarParams)
            _technique->getRenderingProgram()->setUniform (param.first, param.second, true);

        for (auto param : _vec3Params)
            _technique->getRenderingProgram()->setUniform (param.first, param.second, true);

        for (auto param : _vec2Params)
            _technique->getRenderingProgram()->setUniform (param.first, param.second, true);

        unsigned i = 0;
        for (auto texture_ : _textures)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            //glBindSampler (i, GL_LINEAR);
            texture_.second->use();

            _technique->getRenderingProgram()->setUniformSampler (texture_.first, i, true);
            ++i;
        }
    }
}
