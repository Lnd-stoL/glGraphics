
#include "graphics_renderer.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    graphics_renderer::graphics_renderer (render_window::ptr renderWindow)
    {
        renderWindow->frameDrawEvent().handleWith ([this]() { _newFrame(); });
    }


    void graphics_renderer::use (math3d::object2screen_transform_d &&trans)
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


    void graphics_renderer::renderScene (scene::ptr scene, scene::render_group_id renderGroup)
    {
        _scene = scene;
        scene->draw (*this, renderGroup);
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


    void graphics_renderer::draw (gpu_buffer &vertexBuffer, gpu_buffer &indexBuffer, uint8_t bytesPerIndex, unsigned indicesCount)
    {
        if (indicesCount == 0)  indicesCount = indexBuffer.size();
        _frameStatistics._trianglesCount += indicesCount / 3;

        vertexBuffer.use();
        indexBuffer.use();

        _state._material->setup (*this);

        _setupShaderBeforeDraw();

        _beforeDrawCallEvent (*this);
        GLenum indexType = bytesPerIndex == sizeof (uint32_t) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
        glDrawElements (GL_TRIANGLES, indicesCount, indexType, nullptr);

        _frameStatistics._drawCalls++;
    }


    void graphics_renderer::drawPoints (gpu_buffer &vertexBuffer)
    {
        vertexBuffer.use();
        _frameStatistics._trianglesCount += vertexBuffer.size() * 2;

        _state._material->setup (*this);
        _setupShaderBeforeDraw();

        _beforeDrawCallEvent (*this);
        glDrawArrays (GL_POINTS, 0, vertexBuffer.size());

        _frameStatistics._drawCalls++;
    }


    void graphics_renderer::renderTo (render_window &wnd, bool autoClear)
    {
        _state._frameBuffer = nullptr;

        _state._rtWidth = wnd.width();
        _state._rtHeight = wnd.height();

        frame_buffer::useDefault();
        glViewport (0, 0, wnd.width (), wnd.height());
        if (autoClear)  wnd.clear();
    }


    void graphics_renderer::renderTo (frame_buffer::ptr frameBuffer, bool autoClear)
    {
        _state._frameBuffer = frameBuffer;

        _state._rtWidth = frameBuffer->width();
        _state._rtHeight = frameBuffer->height();

        frameBuffer->use();
        glViewport (0, 0, frameBuffer->width(), frameBuffer->height());
        if (autoClear)  frameBuffer->clear();
    }


    void graphics_renderer::_setupShaderBeforeDraw()
    {
        if (_scene)
        {
            _state.activeMaterial()->set ("uLightPos", _scene->sunPosition().convertType<float>());
            _state.activeMaterial()->set ("uLightColor", _scene->sunColor().asVector());
        }

        _state.activeMaterial()->set ("uFrameCount", _frameCount);
        _state.activeMaterial()->set ("uFrameSize", math3d::vector2_f (_state._rtWidth, _state._rtHeight));

        auto viewInt = _state.activeCamera ()->projection ()->viewInterval();
        _state.activeMaterial()->set ("uClipNearFar",
                                      math3d::vector2_f ((float) viewInt.from(), (float) viewInt.to()));

    }


    void graphics_renderer::_newFrame()
    {
        _materialSet = false;
        _frameCount += _frameCountScale;

        _lastFrameStatistics = _frameStatistics;
        _frameStatistics = frame_statistics();
    }


    void graphics_renderer::blend (bool enabled)
    {
        if (enabled && !_state._blend)
        {
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        if (!enabled &&  _state._blend)
        {
            glDisable (GL_BLEND);
        }

        _state._blend = enabled;
    }


    void graphics_renderer::testDepth (bool enabled)
    {
        if (enabled /*&& !_state._testDepth*/)
        {
            glEnable (GL_DEPTH_TEST);
        }

        if (!enabled /*&& _state._testDepth*/)
        {
            glDisable (GL_DEPTH_TEST);
        }

        _state._testDepth = enabled;
    }


    void graphics_renderer::backfaceCulling (bool enabled)
    {
        if (enabled && !_state._backfaceCulling)
        {
            glEnable (GL_CULL_FACE);
        }

        if (!enabled && _state._backfaceCulling)
        {
            glDisable (GL_CULL_FACE);
        }

        _state._backfaceCulling = enabled;
    }


    void graphics_renderer::draw (const renderable &object)
    {
        object.draw (*this);
    }


    void graphics_renderer::writeDepth (bool enabled)
    {
        if (enabled && !_state._writeDepth)
        {
            glDepthMask (GL_TRUE);
        }

        if (!enabled && _state._writeDepth)
        {
            glDepthMask (GL_FALSE);
        }

        _state._writeDepth = enabled;
    }
}
