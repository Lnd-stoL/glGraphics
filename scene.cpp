
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


    void scene::setSun (math3D::vector3_d position)
    {
        _sunPosition = position;
    }

//----------------------------------------------------------------------------------------------------------------------

    graphics_renderer::graphics_renderer (render_window &renderWindow)
    {
        renderWindow.frameDrawEvent().handleWith ([this] (const render_window &) { _newFrame(); });
    }


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
        _materialSet = false;
    }


    void graphics_renderer::renderScene (scene::ptr scene)
    {
        _scene = scene;
        scene->draw (*this);
    }


    void graphics_renderer::stopForcingMaterial()
    {
        _forcedMaterial = false;
    }


    void graphics_renderer::forceMaterial (material::ptr mat)
    {
        _forcedMaterial = false;
        use (mat);
        _forcedMaterial = true;
    }


    void graphics_renderer::draw (gpu_buffer &vertexBuffer, gpu_buffer &indexBuffer)
    {
        vertexBuffer.use();
        indexBuffer.use();

        _state._material->setup (*this);

        _setupShaderBeforeDraw();

        _beforeDrawCallEvent (*this);
        glDrawElements (GL_TRIANGLES, indexBuffer.getSize(), GL_UNSIGNED_SHORT, nullptr);
        debug::gl::test();
    }


    void graphics_renderer::renderTo (render_window &wnd, bool autoClear)
    {
        _state._frameBuffer = nullptr;

        frame_buffer::useDefault();
        glViewport (0, 0, wnd.getWidth(), wnd.getHeight());
        if (autoClear)  wnd.clear();
    }


    void graphics_renderer::renderTo (frame_buffer::ptr frameBuffer, bool autoClear)
    {
        _state._frameBuffer = frameBuffer;

        frameBuffer->use();
        glViewport (0, 0, frameBuffer->getWidth(), frameBuffer->getHeight());
        if (autoClear)  frameBuffer->clear();
    }


    void graphics_renderer::_setupShaderBeforeDraw()
    {
        if (_scene)
        {
            _state.getRenderingProgram()->setUniform ("uLightPos", _scene->getSunPosition().convertType<float>(), true);
            _state.getRenderingProgram()->setUniform ("uFrameCount", _frameCount, true);
        }

        auto viewInt = _state.getCamera()->getProjection()->getViewInterval();
        _state.getRenderingProgram()->setUniform ("uClipNearFar", math3D::vector2_f (viewInt.getFrom(), viewInt.getTo()), true);

    }


    void graphics_renderer::_newFrame()
    {
        _materialSet = false;
        _frameCount += 0.001;
    }
}
