
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

    void graphics_renderer::use (math3D::object2screen_transform_d &&trans)
    {
        _state._object2ScreenTransform = trans;
    }


    void graphics_renderer::use (camera::ptr cam)
    {
        _state._camera = cam;
    }


    void graphics_renderer::use (material::ptr mat)
    {
        if (_forcedMaterial)  return;
        _state._material = mat;
    }


    void graphics_renderer::renderScene (scene::ptr scene)
    {
        _scene = scene;
    }


    void graphics_renderer::stopForcingMaterial()
    {
        _forcedMaterial = false;
    }


    void graphics_renderer::forceMaterial (material::ptr mat)
    {
        _state._material = mat;
        _forcedMaterial = true;
    }


    void graphics_renderer::draw (gpu_buffer &vertexBuffer, gpu_buffer &indexBuffer)
    {
        vertexBuffer.use();
        indexBuffer.use();

        _state._material->setup (*this);

        glDrawElements (GL_TRIANGLES, indexBuffer.getSize(), GL_UNSIGNED_SHORT, nullptr);
        debug::gl::test();
    }
}
