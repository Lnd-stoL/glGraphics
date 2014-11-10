
#include "scene.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    scene::scene() :
            _sceneObjects ([] (const scene_object::ptr& obj1, const scene_object::ptr& obj2)
            {
                return *obj1 < *obj2;
            })
    { }


    void scene::removeSceneObject (scene_object::ptr sceneObject)
    {
        _sceneObjects.erase (sceneObject);
    }


    scene_object::ptr scene::addRenderableObject (renderable::ptr renderableObject, int renderQueue)
    {
        auto sceneObject = scene_object::alloc (renderableObject, renderQueue); // TODO: Ineffective
        _sceneObjects.insert (sceneObject);
        return sceneObject;
    }


    void scene::addSceneObject (scene_object::ptr sceneObject)
    {
        _sceneObjects.insert (sceneObject);
    }


    void scene::draw (graphics_renderer &renderer) const
    {
        for (auto nextObj : _sceneObjects)
        {
            nextObj->getRenderable()->draw (renderer);
        }
    }

//----------------------------------------------------------------------------------------------------------------------

    void graphics_renderer::rendering_state::changeObject2ScreenTrsnaform (math3D::object2screen_transform_d &&trans)
    {
        _object2ScreenTransform = trans;
    }


    void graphics_renderer::rendering_state::changeCamera (camera::ptr cam)
    {
        _camera = cam;
    }


    void graphics_renderer::renderScene (scene::ptr scene)
    {
        _scene = scene;
    }
}
