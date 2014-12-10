
#include "material.hpp"
#include "graphics_renderer.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    technique::technique (gpu_program::ptr renderingProgram) : _gpuProgram (renderingProgram)
    {
        _cacheUniformsLocations();
    }


    void technique::transformNotNeeded()
    {
        _withoutTransform = true;
    }


    void technique::_cacheUniformsLocations()
    {
        if (!_withoutTransform)
        {
            _mvpMatrix_UniformId       = _gpuProgram->uniformLocation ("uMatTransform");
            _worldMatrix_UniformId     = _gpuProgram->uniformLocation ("uMatWorldTransform", true);
            _worldViewMatrix_UniformId = _gpuProgram->uniformLocation ("uMatWorldViewTransform", true);
        }

        _viewPos_UniformId = _gpuProgram->uniformLocation ("uViewPos", true);
    }


    void technique::setup (graphics_renderer &renderer) const
    {
        _gpuProgram->use();

        const math3d::object2screen_transform_d &trans = renderer.state().object2ScreenTransform();

        if (gpu_program::isValidUniformLocation (_mvpMatrix_UniformId))
            _gpuProgram->setUniform (_mvpMatrix_UniformId, trans.asMatrix().convertType<float>());

        if (gpu_program::isValidUniformLocation (_worldMatrix_UniformId))
            _gpuProgram->setUniform (_worldMatrix_UniformId, trans.worldTransform().asMatrix().convertType<float>());

        if (gpu_program::isValidUniformLocation (_worldViewMatrix_UniformId))
            _gpuProgram->setUniform (_worldViewMatrix_UniformId, trans.worldCamTransformMatrix().convertType<float>());

        if (gpu_program::isValidUniformLocation (_viewPos_UniformId))
            _gpuProgram->setUniform (_viewPos_UniformId,
                                     renderer.state().activeCamera()->transform().ttranslation().convertType<float>());
    }

//----------------------------------------------------------------------------------------------------------------------

    void material::setup (graphics_renderer &renderer) const
    {
        _technique->setup (renderer);

        for (auto param : _scalarParams)
        {
            if (gpu_program::isValidUniformLocation (param.second.second))
                _technique->gpuProgram()->setUniform (param.second.second, param.second.first);
        }

        for (auto param : _vec3Params)
        {
            if (gpu_program::isValidUniformLocation (param.second.second))
                _technique->gpuProgram()->setUniform (param.second.second, param.second.first);
        }

        for (auto param : _vec2Params)
        {
            if (gpu_program::isValidUniformLocation (param.second.second))
                _technique->gpuProgram()->setUniform (param.second.second, param.second.first);
        }

        unsigned i = 0;
        for (auto nextTexture : _textures)
        {
            glActiveTexture (GL_TEXTURE0 + i);
            texture::invalidateBinding();
            nextTexture.second.first->use();

            if (gpu_program::isValidUniformLocation (nextTexture.second.second))
                _technique->gpuProgram()->setUniformSampler (nextTexture.second.second, i);
            ++i;
        }
    }


    void material::set (const string &name, texture::ptr txt)
    {
        auto existingIt = _textures.find (name);

        if (existingIt != _textures.end())  existingIt->second.first = txt;
        else
        {
            gpu_program::uniform_id uniformId = _technique->gpuProgram()->uniformLocation (name);
            _textures[name] = { txt, uniformId };
        }
    }


    void material::set (const string &name, float param)
    {
        auto existingIt = _scalarParams.find (name);

        if (existingIt != _scalarParams.end())  existingIt->second.first = param;
        else
        {
            gpu_program::uniform_id uniformId = _technique->gpuProgram()->uniformLocation (name);
            _scalarParams[name] = { param, uniformId };
        }
    }


    void material::set (const string &name, const math3d::vector3_f &param)
    {
        auto existingIt = _vec3Params.find (name);

        if (existingIt != _vec3Params.end())  existingIt->second.first = param;
        else
        {
            gpu_program::uniform_id uniformId = _technique->gpuProgram()->uniformLocation (name);
            _vec3Params[name] = { param, uniformId };
        }
    }


    void material::set (const string &name, const math3d::vector2_f &param)
    {
        auto existingIt = _vec2Params.find (name);

        if (existingIt != _vec2Params.end())  existingIt->second.first = param;
        else
        {
            gpu_program::uniform_id uniformId = _technique->gpuProgram()->uniformLocation (name);
            _vec2Params[name] = { param, uniformId };
        }
    }
}
