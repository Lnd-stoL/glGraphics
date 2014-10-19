
#include "material.hpp"
using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    /*virtual*/ void material::use() const
    {
        _renderingProgram->use();
    }


    /*virtual*/ void material::setupViewerTransform (const math3D::object2screen_transform_f &trans)
    {
        _renderingProgram->setUniform ("uMatWorldTransform", trans.getWorldTransform().asMatrix(), true);
        _renderingProgram->setUniform ("uViewPos", trans.getCameraTransform().getTranslation(), true);
        _renderingProgram->setUniform ("uMatTransform", trans.asMatrix());
    }


    /*virtual*/ void textured_material::use() const
    {
        material::use();
        _texture->use();
    }


    /*virtual*/ void textured_material::setupViewerTransform (const math3D::object2screen_transform_f &trans)
    {
        material::setupViewerTransform (trans);
        //_renderingProgram->setUniformSampler ("uTexture", 0);
    }
}