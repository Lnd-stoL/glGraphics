
#include <SFML/Graphics.hpp>

#include "gpu_program.hpp"
#include "mesh.hpp"
#include "exs3d.hpp"
#include "render_window.hpp"
#include "fps_camera.hpp"
#include "scene_renderer.hpp"
#include "mesh_renderable_object.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    render_window window (1200, 900, "OpenGL Graphics");

    perspective_projection_d projection (angle_d::pi / 4, window.getAspectRatio(), interval_d (0.3, 10000));
    render::camera::ptr camera = render::camera::alloc (projection);
    camera->syncProjectionAspectRatio (window.sizeChangedEvent());
    fps_camera_controller cameraController (window, camera);

    auto mesh = render::exs3d_mesh::alloc ("/home/leonid/Загрузки/3d1/gaz3d.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    auto mesh2 = render::exs3d_mesh::alloc ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d");
    mesh->getMesh()->removeComponent ("Cube");
    mesh2->getMesh()->removeComponent ("Cube");

    rotation_d f (vector3_d (1, 0, 0), angle_d::pi / 2);
    mesh_renderable_object::ptr ro = mesh_renderable_object::alloc (mesh->getMesh(),  transform_d (vector3_d (0, 0, 0), f));

    mesh_renderable_object::ptr ro2 =
            mesh_renderable_object::alloc (mesh2->getMesh(),  transform_d (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04)));

    render::scene_renderer renderer (window);
    renderer.addSceneObject (ro2, 0);
    renderer.addSceneObject (ro, 1);

    window.frameDrawEvent().handleWith ([&renderer, camera, &cameraController] (const render_window&) {
        renderer.draw (*camera);
    });

    window.runEventLoop();
    return 0;
}
