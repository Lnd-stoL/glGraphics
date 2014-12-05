
#include "material.hpp"
#include "graphics_renderer.hpp"

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
        const math3d::object2screen_transform_d &trans = renderer.state().object2ScreenTransform();

        _renderingProgram->use();

        if (!_withoutTransform)
        {
            _renderingProgram->setUniform ("uMatWorldTransform", trans.worldTransform().asMatrix().convertType<float>(), true);
            _renderingProgram->setUniform ("uMatTransform", trans.asMatrix().convertType<float>());
            _renderingProgram->setUniform ("uMatWorldViewTransform", trans.worldCamTransformMatrix().convertType<float>(), true);
        }

        _renderingProgram->setUniform ("uViewPos",
                                       renderer.state().activeCamera()->transform().ttranslation().convertType<float>(), true);
    }


    void material::setup (graphics_renderer &renderer) const
    {
        _technique->setup (renderer);
        for (auto param : _scalarParams)
            _technique->renderingProgram()->setUniform (param.first, param.second, true);

        for (auto param : _vec3Params)
            _technique->renderingProgram()->setUniform (param.first, param.second, true);

        for (auto param : _vec2Params)
            _technique->renderingProgram()->setUniform (param.first, param.second, true);

        unsigned i = 0;
        for (auto texture_ : _textures)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            //glBindSampler (i, (GLuint) GL_LINEAR);
            texture::invalidateBinding();
            texture_.second->use();

            _technique->renderingProgram()->setUniformSampler (texture_.first, i, true);
            ++i;
        }
    }
}
