
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

    frame_buffer fb;
    texture::ptr rt = texture::alloc (1200, 900);
    fb.attachColorTexture (rt);

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (angle_d::pi / 4, window.getAspectRatio(), interval_d (0.3, 10000)));
    render::camera::ptr camera = render::camera::alloc (std::move (projection));
    camera->syncProjectionAspectRatio (window.sizeChangedEvent());
    fps_camera_controller cameraController (window, camera);

    render::resources res;

    auto mesh = res.getMeshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", res);
    auto mesh3 = res.getMeshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", res);
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    auto mesh2 = res.getMeshesManager().request ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d", res);
    mesh->getMesh()->removeComponent ("Cube");
    mesh2->getMesh()->removeComponent ("Cube");

    rotation_d f (vector3_d (1, 0, 0), angle_d::pi / 2);
    mesh_renderable_object::ptr ro = mesh_renderable_object::alloc (mesh->getMesh(), transform_d (vector3_d (0, 0, 0), f));

    mesh_renderable_object::ptr ro2 =
            mesh_renderable_object::alloc (mesh2->getMesh(), transform_d (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04)));

    mesh_renderable_object::ptr ro3 =
            mesh_renderable_object::alloc (mesh3->getMesh(), transform_d (vector3_d (20, 0, 0), f));

    render::scene_renderer renderer (window);
    renderer.addSceneObject (ro2, 0);
    renderer.addSceneObject (ro, 1);
    //renderer.addSceneObject (ro3, 10);

    window.frameDrawEvent().handleWith ([&renderer, camera, mesh, &fb, rt, &cameraController] (const render_window&) {
        //fb.use();
        glViewport (0, 0, 1200, 900);

        frame_buffer::useDefault();
        //auto lightCamera = render::camera::alloc (camera->getProjection());
        //lightCamera->addTransform (transform_d (vector3_d (-10, 50, 50), rotation_d()));
        renderer.draw (*camera);

        //rt->saveToFile ("test.jpg");
    });

    window.runEventLoop();
    return 0;
}
