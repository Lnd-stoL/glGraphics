
#ifndef __scene_renderer__included__
#define __scene_renderer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "renderable.hpp"
#include "camera.hpp"
#include "render_window.hpp"
#include "material.hpp"
#include "frame_buffer.hpp"

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
        property_get_ref (Objects, _sceneObjects)
        property_get_ref (SunPosition, _sunPosition)
        property_get_ref (SunColor,  _sunColor);


    public:
        declare_ptr_alloc (scene)

        scene();

        scene_object::ptr addRenderableObject (renderable::ptr renderableObject, int renderQueue = 0);
        void addSceneObject (scene_object::ptr sceneObject);
        void removeSceneObject (scene_object::ptr sceneObject);
        void setSun (math3d::vector3_d position, color_rgb<float> color);

        void draw (graphics_renderer &renderer) const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class graphics_renderer
    {
    public:
        class rendering_state
        {
            math3d::object2screen_transform_d  _object2ScreenTransform;
            camera::ptr  _camera;
            material::ptr  _material;
            frame_buffer::ptr  _frameBuffer;


        public:
            property_get_ref (Object2ScreenTransform, _object2ScreenTransform)
            property_get (Camera, _camera)
            property_get (Material, _material)
            property_get (RenderingProgram, _material->getTechnique()->getRenderingProgram())
            property_get (FrameBuffer, _frameBuffer)

        public:
            friend class graphics_renderer;
        };

//----------------------------------------------------------------------------------------------------------------------

        class frame_statistics
        {
            unsigned  _drawCalls = 0;

        public:
            property_get (DrawCalls, _drawCalls)

        public:
            friend class graphics_renderer;
        };

//----------------------------------------------------------------------------------------------------------------------

    private:
        rendering_state  _state;
        frame_statistics  _lastFrameStatistics;
        frame_statistics  _frameStatistics;
        scene::ptr _scene;

        static constexpr double _frameCountScale = 0.001;
        float _frameCount = 0;

        bool  _forcedMaterial = false;
        bool  _materialSet = false;

        event<graphics_renderer&>  _beforeDrawCallEvent;


    public:
        property_ref (state, _state)
        property_get_ref (FrameStatistics, _frameStatistics)
        property_get_ref (LastFrameStatistics, _lastFrameStatistics)
        property_get (Scene, _scene)
        property_get (FrameCount, _frameCount / _frameCountScale)

        event_access (beforeDrawCall, _beforeDrawCallEvent)


    protected:
        void _setupShaderBeforeDraw();
        inline void _newFrame();

    public:
        declare_ptr_alloc (graphics_renderer)

        graphics_renderer (render_window::ptr renderWindow);

        void use (math3d::object2screen_transform_d &&trans);
        void use (camera::ptr cam);
        void use (material::ptr mat);

        void renderTo (frame_buffer::ptr frameBuffer, bool autoClear = true);
        void renderTo (render_window &wnd, bool autoClear = true);

        void forceMaterial (material::ptr mat);
        void stopForcingMaterial();

        void draw (gpu_buffer &vertexBuffer, gpu_buffer &indexBuffer, uint8_t bytesPerIndex = 2, unsigned indicesCount = 0);

        void renderScene (scene::ptr sceneToRender);
    };

}

//----------------------------------------------------------------------------------------------------------------------

#endif
