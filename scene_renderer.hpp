
#ifndef __scene_renderer__included__
#define __scene_renderer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "renderable.hpp"
#include "camera.hpp"

#include <GL/glew.h>
#include <set>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class scene_object
    {
        int             _renderQueue;
        renderable::ptr _renderableObject;

    public:
        property_get (RenderQueue, _renderQueue)
        property_get (Renderable, _renderableObject)

    public:
        declare_ptr_alloc (scene_object)

        scene_object (renderable::ptr renderableObject, int renderQueue = 0) : _renderQueue (renderQueue),
                                                                               _renderableObject (renderableObject)
        { }

        bool operator< (const scene_object& theOtherSceneObject)
        {
            return _renderQueue < theOtherSceneObject._renderQueue;
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    class scene_renderer
    {
        std::set<scene_object::ptr, std::function<bool (const scene_object::ptr&, const scene_object::ptr&)>> _sceneObjects;

    public:
        property_get_ref (SceneObjects, _sceneObjects)

    public:
        declare_ptr_alloc (scene_renderer)
        scene_renderer();

        scene_object::ptr addSceneObject (renderable::ptr renderableObject, int renderQueue = 0);
        void addSceneObject (scene_object::ptr sceneObject);
        void removeSceneObject (scene_object::ptr sceneObject);

        void draw (const camera &viewer) const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
