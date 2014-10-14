
#include <SFML/Graphics.hpp>

#include "gpu_program.hpp"
#include "mesh.hpp"
#include "exs3d.hpp"
#include "render_window.hpp"
#include "fps_camera.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    render_window window (900, 700, "OpenGL Graphics");

    render::camera::ptr camera = render::camera::alloc();
    fps_camera_controller cameraController (window, camera);

    render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/gaz3d.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d");
    mesh.getMesh()->removeComponent ("Cube");

    window.frameDrawEvent().handleWith ([&mesh, camera, &cameraController] (const render_window&) {
        rotation_f f (vector3_f (1, 0, 0), angle_f::pi / 2);
        object2screen_transform_f t (
            transform_f (vector3_f (0, 0, 0), f),
            camera->getTransform().convertType<float>(), perspective_projection_f (angle_f::pi / 4, 900.0f / 700.0f, interval_f (0.3, 10000)));

        auto tm = t.asMatrix();

        for (auto component : mesh.getMesh()->getComponents())
        {
            auto shader = component->getMaterial()->getRenderingProgram();
            shader->setUniformSampler ("uTexture", 0);
            shader->setUniform ("uViewPos", camera->getTransform().getTranslation().convertType<float>());
            shader->setUniform ("uMatTransform", tm);
            shader->setUniform ("uMatWorldTransform", t.getWorldTransform().asMatrix());
        }

        mesh.draw();
    });

    window.runEventLoop();
    return 0;
}
