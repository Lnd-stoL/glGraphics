
#include "transformable_object.hpp"
#include "scene.hpp"

//----------------------------------------------------------------------------------------------------------------------

transformable_renderable_object::transformable_renderable_object (const transform_d &initialTransform) :
    _transform (initialTransform)
{

}


void transformable_renderable_object::_setupObject2ScreenTransform (graphics_renderer &renderer) const
{
    auto usedCamera = renderer.state().getCamera();
    renderer.use (object2screen_transform_d (_transform, usedCamera->getInversedTransform(), usedCamera->getProjection()));
}
