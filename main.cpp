
#include <SFML/Graphics.hpp>

#include "gpu_program.hpp"
#include "mesh.hpp"
#include "exs3d.hpp"

using oo_extensions::mkstr;
using namespace math3D;

/*
** Function called when the window is created or resized
*/
void ReshapeFunc(int width, int height)
{
  glMatrixMode (GL_PROJECTION);

  glLoadIdentity();
  gluPerspective (45, width / (float) height, 1, 150);
  //glFrustum(-1, 1, -1, 1, 1, 100000);
  glViewport (0, 0, width, height);

  glMatrixMode (GL_MODELVIEW);
}


int main (int argc, char **argv)
{
    sf::RenderWindow window (sf::VideoMode (900, 700), "OpenGL Graphics", sf::Style::Default, sf::ContextSettings (24));
    window.setFramerateLimit (60);
    //window.setVerticalSyncEnabled (true);

    glewInit();

    render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/gaz3d.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d");

    mesh.getMesh()->removeComponent ("Cube");

    transform_d camera = transform_d (vector3_d (0, 0, 0), rotation_d());
    auto lastMousePos = sf::Mouse::getPosition (window);

    float alpha = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent (event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::Resized)
            {
                ReshapeFunc (event.size.width, event.size.height);
                //glViewport(0, 0, event.size.width, event.size.height);
            }

            else if (event.type == sf::Event::KeyPressed)
            {
                  if (event.key.code == sf::Keyboard::Escape) {
                      window.close ();
                  }
            }
        }


        auto curMousePos  = sf::Mouse::getPosition (window);

        if (sf::Keyboard::isKeyPressed (sf::Keyboard::W))
        {
            vector3_d forward (0, 0, -1);
            forward = camera.getRotation().inversed().apply (forward);
            forward /= 10;
            camera = transform_d (camera.getTranslation() + forward, camera.getRotation());
        }

        if (sf::Keyboard::isKeyPressed (sf::Keyboard::S))
        {
            vector3_d forward (0, 0, 1);
            forward = camera.getRotation().inversed().apply (forward);
            forward /= 10;
            camera = transform_d (camera.getTranslation() + forward, camera.getRotation());
        }

        if (sf::Keyboard::isKeyPressed (sf::Keyboard::A))
        {
            vector3_d forward (-1, 0, 0);
            forward = camera.getRotation().inversed().apply (forward);
            forward /= 10;
            camera = transform_d (camera.getTranslation() + forward, camera.getRotation());
        }

        if (sf::Keyboard::isKeyPressed (sf::Keyboard::D))
        {
            vector3_d forward (1, 0, 0);
            forward = camera.getRotation().inversed().apply (forward);
            forward /= 10;
            camera = transform_d (camera.getTranslation() + forward, camera.getRotation());
        }


        if (sf::Keyboard::isKeyPressed (sf::Keyboard::Left))
        {
            vector3_d xvec (0, 1, 0);
            //xvec = camera.getRotation().inversed().apply (xvec);
            rotation_d rotx (xvec, -1 / 100.0f);

            rotation_d camRot = camera.getRotation();
            camRot.combine (rotx);
            camera = transform_d (camera.getTranslation(), camRot);
        }


        if (sf::Keyboard::isKeyPressed (sf::Keyboard::Right))
        {
            vector3_d xvec (0, 1, 0);
            //xvec = camera.getRotation().apply (xvec);
            rotation_d rotx (xvec, 1 / 100.0f);

            rotation_d camRot = camera.getRotation();
            camRot.combine (rotx);
            camera = transform_d (camera.getTranslation(), camRot);
        }


        if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up))
        {
            vector3_d xvec (1, 0, 0);
            //xvec = camera.getRotation().apply (xvec);
            rotation_d rotx (xvec, -1 / 100.0f);

            rotation_d camRot = camera.getRotation();
            camRot.combine (rotx);
            camera = transform_d (camera.getTranslation(), camRot);
        }


        if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down))
        {
            vector3_d xvec (1, 0, 0);
            //xvec = camera.getRotation().inversed().apply (xvec);
            rotation_d rotx (xvec, 1 / 100.0f);

            rotation_d camRot = camera.getRotation();
            camRot.combine (rotx);
            camera = transform_d (camera.getTranslation(), camRot);
        }



        if (sf::Mouse::isButtonPressed (sf::Mouse::Left))
        {
            float deltaX = curMousePos.x - lastMousePos.x;
            float deltaY = curMousePos.y - lastMousePos.y;

            //if (deltaX < 10 && deltaY < 10)
            {
                vector3_d xvec (1, 0, 0);
                vector3_d yvec (0, 1, 0);
                rotation_d rotx (xvec, deltaY / 400.0f);
                //yvec = rotx.inversed().apply (yvec);
                rotation_d roty (yvec, deltaX / 400.0f);

                auto camRot = camera.getRotation();
                camRot.combine (rotx);
                camRot.combine (roty);
                camera = transform_d (camera.getTranslation(), camRot);
            }

            //sf::Mouse::setPosition (lastMousePos, window);
        }

        lastMousePos = curMousePos;




        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        //alpha += 0.04;
        rotation_f f (vector3_f (1, 0, 0), angle_f::pi / 2);
        //rotation_f r (rotation_f (vector3_f (0, 1, 0), alpha));

        object2screen_transform_f t (
            transform_f (vector3_f (0, 0, 0), f), camera.convertType<float>(), perspective_projection_f (angle_f::pi / 4, 900.0 / 700.0, interval_f (0.3, 10000)));

        auto tm = t.asMatrix();


        for (auto component : mesh.getMesh()->getComponents())
        {
            auto shader = component->getMaterial()->getRenderingProgram();
            shader->setUniformSampler ("uTexture", 0);
            shader->setUniform ("uViewPos", camera.getTranslation().convertType<float>());
            shader->setUniform ("uMatTransform", tm);
            shader->setUniform ("uMatWorldTransform", t.getWorldTransform().asMatrix());
        }

        //debug::log::println (t.getWorld
        //
        //
        //
        //
        //
        //
        //
        // Transform().asMatrix().asString());

        glClearColor (1, 1, 1, 1);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.draw();

        glFlush();
        window.display();
    }

    return 0;
}
