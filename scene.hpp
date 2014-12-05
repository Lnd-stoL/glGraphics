
#ifndef __scene__included__
#define __scene__included__

//----------------------------------------------------------------------------------------------------------------------

#include "renderable.hpp"
#include "material.hpp"
#include <set>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class graphics_renderer;

    class scene_object
    {
        int             _renderQueue;
        renderable::ptr _renderableObject;

    public:
        property_get (renderQueue, _renderQueue)
        property_get (underlyingRenderable,  _renderableObject)

    public:
        declare_ptr_alloc (scene_object)

        scene_object (renderable::ptr renderableObject, int renderQueue = 0) : _renderQueue (renderQueue),
                                                                               _renderableObject (renderableObject)
        { }


        bool operator< (const scene_object& theOtherSceneObject)
        {
            if (_renderQueue != theOtherSceneObject._renderQueue)
                return _renderQueue < theOtherSceneObject._renderQueue;

            return ((const renderable*) _renderableObject.get()) < ((const renderable*) theOtherSceneObject._renderableObject.get());
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    class scene : public renderable
    {
        std::set<scene_object::ptr,
                std::function<bool (const scene_object::ptr&, const scene_object::ptr&)>> _sceneObjects;

        math3d::vector3_d  _sunPosition;
        color_rgb<float>  _sunColor;

    public:
        property_get_ref (objects, _sceneObjects)
        property_get_ref (sunPosition, _sunPosition)
        property_get_ref (sunColor,  _sunColor);


    public:
        declare_ptr_alloc (scene)

        scene();

        scene_object::ptr addRenderableObject (renderable::ptr renderableObject, int renderQueue = 0);
        void addSceneObject (scene_object::ptr sceneObject);
        void removeSceneObject (scene_object::ptr sceneObject);
        void setSun (math3d::vector3_d position, color_rgb<float> color);

        void draw (graphics_renderer &renderer) const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
