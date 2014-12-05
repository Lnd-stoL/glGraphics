
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
        auto sceneObject = scene_object::alloc (renderableObject, renderQueue);     // TODO: Ineffective
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
            nextObj->underlyingRenderable()->draw (renderer);
        }
    }


    void scene::setSun (math3d::vector3_d position, color_rgb<float> color)
    {
        _sunPosition = position;
        _sunColor = color;
    }
}
