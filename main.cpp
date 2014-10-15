
#include <SFML/Graphics.hpp>

#include "gpu_program.hpp"
#include "mesh.hpp"
#include "exs3d.hpp"
#include "render_window.hpp"
#include "fps_camera.hpp"
#include "scene_renderer.hpp"
#include "mesh_renderable_object.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    render_window window (900, 700, "OpenGL Graphics");

    perspective_projection_d projection (angle_d::pi / 4, 900.0 / 700.0, interval_d (0.3, 10000));
    render::camera::ptr camera = render::camera::alloc (projection);
    fps_camera_controller cameraController (window, camera);

    auto mesh = render::exs3d_mesh::alloc ("/home/leonid/Загрузки/3d1/gaz3d.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d");
    mesh->getMesh()->removeComponent ("Cube");

    rotation_d f (vector3_d (1, 0, 0), angle_d::pi / 2);
    mesh_renderable_object::ptr ro = mesh_renderable_object::alloc (mesh->getMesh(),  transform_d (vector3_d (0, 0, 0), f));

    render::scene_renderer renderer;
    renderer.addSceneObject (ro, 0);
    //renderer.addSceneObject (mesh, 1);

    window.frameDrawEvent().handleWith ([&renderer, camera, &cameraController] (const render_window&) {
        renderer.draw (*camera);
    });

    window.runEventLoop();
    return 0;
}
