
#include "gpu_program.hpp"
#include "mesh.hpp"
#include "exs3d.hpp"
#include "render_window.hpp"
#include "fps_camera.hpp"
#include "scene_renderer.hpp"
#include "mesh_renderable_object.hpp"
#include "resource_manager_impl.hpp"
#include "frame_buffer.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    render_window window (1200, 900, "OpenGL Graphics");

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (angle_d::pi / 4, window.getAspectRatio(), interval_d (0.3, 10000)));
    render::camera::ptr camera = render::camera::alloc (std::move (projection));
    camera->syncProjectionAspectRatio (window.sizeChangedEvent());
    fps_camera_controller cameraController (window, camera);

    render::resources renderRes;
    auto mesh = renderRes.getMeshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", renderRes);
    auto mesh3 = renderRes.getMeshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", renderRes);
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    auto mesh2 = renderRes.getMeshesManager().request ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d", renderRes);
    mesh->getMesh()->removeComponent ("Cube");
    mesh2->getMesh()->removeComponent ("Cube");

    rotation_d f (vector3_d (1, 0, 0), angle_d::pi / 2);
    mesh_renderable_object::ptr sceneObj1 = mesh_renderable_object::alloc (mesh->getMesh(), transform_d (vector3_d (0, 0, 0), f));

    mesh_renderable_object::ptr sceneObj2 =
            mesh_renderable_object::alloc (mesh2->getMesh(), transform_d (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04)));

    mesh_renderable_object::ptr sceneObj3 =
            mesh_renderable_object::alloc (mesh3->getMesh(), transform_d (vector3_d (20, 0, 0), f));

    render::scene_renderer sceneRenderer (window);
    sceneRenderer.addSceneObject (sceneObj2, 0);
    sceneRenderer.addSceneObject (sceneObj1, 1);
    sceneRenderer.addSceneObject (sceneObj3, 1);

    frame_buffer shadowMapFrameBuffer (window.getWidth(), window.getHeight());
    texture::ptr shadowMapDepthTexture = shadowMapFrameBuffer.attachDepthTexture();
    //texture::ptr rt = shadowMapFrameBuffer.attachColorTexture();

    auto shadowMapGenProgId = gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(),
                                               "/home/leonid/Dev/glGraphics/shadowmap_gen.vert", "/home/leonid/Dev/glGraphics/shadowmap_gen.frag");
    auto shadowMapGenProg = renderRes.getGpuProgramsManager().request (shadowMapGenProgId);

    window.frameDrawEvent().handleWith ([&] (const render_window& window) {
        shadowMapFrameBuffer.use();
        //glViewport (0, 0, 1200, 900);

        glDrawBuffer (GL_NONE);
        glReadBuffer (GL_NONE);

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        glClearColor (1, 1, 1, 1);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //shadowMapFrameBuffer.use();

        if (!shadowMapFrameBuffer.readyForRender())
        {
            debug::log::println ("shit");
        }

        unique_ptr<orthographic_projection_d> lightProj (
                new orthographic_projection_d (70, window.getAspectRatio(), interval_d (1, 10000)));
        auto lightCamera = render::camera::alloc (std::move (lightProj));
        lightCamera->addTransform (transform_d (vector3_d (-10, 50, 50), rotation_d (vector3_d (1, 0, 0), angle_d::pi / 6.0)));

        sceneRenderer.draw (*lightCamera);

        //rt->saveToFile ("test.jpg");
        //rt->saveToFile ("test-depth.jpg");

        // ---------------------------------------------------------------------------------------------  Render pass

        frame_buffer::useDefault();

        glDrawBuffer (GL_BACK);
        glReadBuffer (GL_BACK);

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        glClearColor (1, 1, 1, 1);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto p = dynamic_pointer_cast<textured_material>(mesh->getMesh()->getComponents()[0]->getMaterial());
        p->changeTexture (shadowMapDepthTexture);
        sceneRenderer.draw (*camera);

        //dt->saveToFile ("test-depth.jpg");
        //while (!sf::Keyboard::isKeyPressed (sf::Keyboard::Space));
    });


    debug::log::println ("===================================== loading finished ============================================");
    window.runEventLoop();
    debug::log::println ("==================================== rendering finished ===========================================");

    return 0;
}
