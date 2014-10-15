
#include "scene_renderer.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    scene_renderer::scene_renderer() :
            _sceneObjects ([] (const scene_object::ptr& obj1, const scene_object::ptr& obj2)
            {
                return *obj1 < *obj2;
            })
    { }


    void scene_renderer::removeSceneObject (scene_object::ptr sceneObject)
    {
        _sceneObjects.erase (sceneObject);
    }


    scene_object::ptr scene_renderer::addSceneObject (renderable::ptr renderableObject, int renderQueue)
    {
        auto sceneObject = scene_object::alloc (renderableObject, renderQueue); // TODO: Ineffective
        _sceneObjects.insert (sceneObject);
        return sceneObject;
    }


    void scene_renderer::addSceneObject (scene_object::ptr sceneObject)
    {
        _sceneObjects.insert (sceneObject);
    }


    void scene_renderer::draw (const camera &viewer) const
    {
        for (auto nextObj : _sceneObjects)
        {
            nextObj->getRenderable()->draw (viewer);
        }
    }
}
