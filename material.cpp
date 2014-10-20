
#include "material.hpp"
using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    /*virtual*/ void material::use() const
    {
        _renderingProgram->use();
    }


    /*virtual*/ void material::setupViewerTransform (const math3D::object2screen_transform_d &trans)
    {
        _renderingProgram->setUniform ("uMatWorldTransform", trans.getWorldTransform().asMatrix().convertType<float>(), true);
        _renderingProgram->setUniform ("uViewPos", trans.getCameraTransform().getTranslation().convertType<float>(), true);
        _renderingProgram->setUniform ("uMatTransform", trans.asMatrix().convertType<float>());
    }


    /*virtual*/ void textured_material::use() const
    {
        material::use();
        _texture->use();
    }


    /*virtual*/ void textured_material::setupViewerTransform (const math3D::object2screen_transform_d &trans)
    {
        material::setupViewerTransform (trans);
        //_renderingProgram->setUniformSampler ("uTexture", 0);
    }
}
