
#ifndef __scene_renderer__included__
#define __scene_renderer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "renderable.hpp"
#include "camera.hpp"
#include "render_window.hpp"
#include "material.hpp"
#include "frame_buffer.hpp"

#include <GL/glew.h>
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
        property_get (RenderQueue, _renderQueue)
        property_get (Renderable,  _renderableObject)

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

    class scene : public renderable
    {
        std::set<scene_object::ptr,
                std::function<bool (const scene_object::ptr&, const scene_object::ptr&)>> _sceneObjects;

    public:
        property_get_ref (Objects, _sceneObjects)


    public:
        declare_ptr_alloc (scene)

        scene();

        scene_object::ptr addRenderableObject (renderable::ptr renderableObject, int renderQueue = 0);
        void addSceneObject (scene_object::ptr sceneObject);
        void removeSceneObject (scene_object::ptr sceneObject);

        void draw (graphics_renderer &renderer) const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class graphics_renderer
    {
    public:
        class rendering_state
        {
            math3D::object2screen_transform_d  _object2ScreenTransform;
            camera::ptr  _camera;
            material::ptr  _material;
            frame_buffer::ptr  _frameBuffer;


        public:
            property_get_ref (Object2ScreenTransform, _object2ScreenTransform);
            property_get (Camera, _camera);
            property_get (Material, _material);
            property_get (RenderingProgram, _material->getTechnique()->getRenderingProgram());
            property_get (FrameBuffer, _frameBuffer);

        public:
            friend class graphics_renderer;
        };

//----------------------------------------------------------------------------------------------------------------------

    private:
        rendering_state _state;
        scene::ptr _scene;
        bool  _forcedMaterial = false;

        event<graphics_renderer&>  _beforeDrawCallEvent;


    public:
        property_ref (state, _state);
        property_get (Scene, _scene);

        event_access (beforeDrawCall, _beforeDrawCallEvent);


    public:
        declare_ptr_alloc (graphics_renderer)

        void use (math3D::object2screen_transform_d &&trans);
        void use (camera::ptr cam);
        void use (material::ptr mat);

        void renderTo (frame_buffer::ptr frameBuffer);
        void renderTo (render_window &wnd);

        void forceMaterial (material::ptr mat);
        void stopForcingMaterial();

        void draw (gpu_buffer &vertexBuffer, gpu_buffer &indexBuffer);

        void renderScene (scene::ptr sceneToRender);
    };

}

//----------------------------------------------------------------------------------------------------------------------

#endif
