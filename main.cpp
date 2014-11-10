
#include "gpu_program.hpp"
#include "mesh.hpp"
#include "exs3d.hpp"
#include "render_window.hpp"
#include "fps_camera.hpp"
#include "scene.hpp"
#include "mesh_renderable_object.hpp"
#include "resource_manager_impl.hpp"
#include "frame_buffer.hpp"
#include "render_resources.hpp"
#include "render_resources_impl.hpp"

using oo_extensions::mkstr;
using namespace math3D;

//----------------------------------------------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    debug::log::println ("initializing OpenGL render window ...");
    render_window window (1200, 900, "OpenGL Graphics");

    unique_ptr<perspective_projection_d> projection (new perspective_projection_d (angle_d::pi / 4, window.getAspectRatio(), interval_d (1, 1000)));
    render::camera::ptr camera = render::camera::alloc (std::move (projection));
    camera->addTransform (transform_d (vector3_d (0, 10, 0), rotation_d (vector3_d (1, 0, 0), angle_d::pi / 2)));
    camera->syncProjectionAspectRatio (window.sizeChangedEvent());
    fps_camera_controller cameraController (window, camera);
    cameraController.setTranslationSpeed (cameraController.getTranslationSpeed() * 3);

    //unique_ptr<orthographic_projection_d> lightProj (
    //        new orthographic_projection_d (10, window.getAspectRatio(), interval_d (1, 5000)));
    unique_ptr<perspective_projection_d> lightProj (new perspective_projection_d (angle_d::pi / 4, window.getAspectRatio(), interval_d (1, 500)));
    auto lightCamera = render::camera::alloc (std::move (lightProj));
    lightCamera->addTransform (transform_d (vector3_d (0, 20, 20), rotation_d (vector3_d (1, 0, 0), angle_d::pi / 5)));

    render::resources renderRes;
    //exs3d_loader loader;
    //auto binMesh = loader.loadMesh ("/home/leonid/Загрузки/3d1/gaz3d_bin.exs3d", renderRes);
    //auto binMesh = renderRes.meshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", renderRes)->getMesh();
    //auto mesh = renderRes.meshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", renderRes);
    //auto mesh3 = renderRes.meshesManager().request ("/home/leonid/Загрузки/3d1/gaz3d.exs3d", renderRes);
    //render::exs3d_mesh mesh ("/home/leonid/Загрузки/3d1/DefenderLingerie00.exs3d");
    auto mesh2 = renderRes.requestFromFile<exs3d_mesh> ("/home/leonid/Загрузки/Small Tropical Island/untitled.exs3d");
    //mesh->getMesh()->removeComponent ("Cube");

    mesh2->getRenderableMesh()->removeComponent ("Cube");
    //binMesh->removeComponent ("Cube");

    rotation_d f (vector3_d (1, 0, 0), angle_d::pi / 2);
    //mesh_renderable_object::ptr sceneObj1 = mesh_renderable_object::alloc (binMesh, transform_d (vector3_d (0, 0, 0), f));
    //
    mesh_renderable_object::ptr sceneObj2 =
            mesh_renderable_object::alloc (mesh2->getRenderableMesh(), transform_d (vector3_d (0, 0, 0), rotation_d(), vector3_d (0.04)));

    //mesh_renderable_object::ptr sceneObj3 =
    //        mesh_renderable_object::alloc (mesh3->getMesh(), transform_d (vector3_d (20, 0, 0), f));

    auto testScene = scene::alloc();
    //testScene->addRenderableObject (sceneObj2, 0);
    //testScene->addRenderableObject (sceneObj1, 1);
    //testScene->addRenderableObject (sceneObj3, 1);

    graphics_renderer renderer;

    frame_buffer shadowMapFrameBuffer (window.getWidth() * 2, window.getHeight() * 2);
    texture::ptr shadowMapDepthTexture = shadowMapFrameBuffer.attachDepthTexture();
    //texture::ptr rt = shadowMapFrameBuffer.attachColorTexture();

    auto shadowMapGenProgId = gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(),
                                               "/home/leonid/Dev/glGraphics/shadowmap_gen.vert", "/home/leonid/Dev/glGraphics/shadowmap_gen.frag");
    auto shadowMapGenProg = renderRes.gpuProgramsManager().request (shadowMapGenProgId);

    renderRes.fontsManager().addFileSearchLocation ("resources/fonts/");

    font::ptr textFont = renderRes.fontsManager().request ("default.ttf");
    sf::Text text ("CPU frame time: 1ms", *((sf::Font*)textFont.get()), 15);
    text.setColor (sf::Color (255, 0, 0));
    text.setPosition (5.0f, 5.0f);


    window.frameDrawEvent().handleWith ([&] (const render_window& windoww) {
        shadowMapFrameBuffer.use();
        //glViewport (0, 0, 1200, 900);
        glViewport (0, 0, 1200*2, 900*2);

        glDrawBuffer (GL_NONE);
        glReadBuffer (GL_NONE);

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        glClearColor (1, 1, 1, 1);
        glClear (GL_DEPTH_BUFFER_BIT);

        //shadowMapFrameBuffer.use();

        if (!shadowMapFrameBuffer.readyForRender())
        {
            debug::log::println ("shit");
        }

        object2screen_transform_d shadowMapTranfrom (sceneObj2->getTransform(), lightCamera->getTransform(), camera->getProjection());
        matrix_4x4_f matBias (0.5f, 0.5f, 0.5f, 1.0f);
        matBias.setRow3 (3, 0.5f, 0.5f, 0.5f, 0.5f);
        //debug::log::println (matBias.asString());
        auto matShadow = shadowMapTranfrom.asMatrix().convertType<float>();
        matBias.multiply (matShadow);

        //sceneObj2->getMesh()->getComponents()[0]->getMaterial()->getRenderingProgram()->setUniform ("uShadowmapTransform", matBias);

        renderer.state().changeCamera (lightCamera);
        sceneObj2->draw (renderer);
        //sceneObj1->draw (renderer);

        // ---------------------------------------------------------------------------------------------  Render pass

        frame_buffer::useDefault();
        glViewport (0, 0, 1200, 900);

        glDrawBuffer (GL_BACK);
        glReadBuffer (GL_BACK);

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        glClearColor (1, 1, 1, 1);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //glActiveTexture (GL_TEXTURE0);


        sceneObj2->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniform ("uShadowmapTransform", matShadow, true);
        glActiveTexture (GL_TEXTURE0 + 4);
        shadowMapDepthTexture->use();
        glBindSampler (4, GL_LINEAR);

        sceneObj2->getMesh()->getComponents()[0]->getMaterial()->getTechnique()->getRenderingProgram()->setUniformSampler ("uShadowMap", 4, true);

        //auto p = dynamic_pointer_cast<textured_material>(mesh->getMesh()->getComponents()[0]->getMaterial());
        //p->changeTexture (shadowMapDepthTexture);
        //sceneRenderer.draw (*camera);
        glActiveTexture (GL_TEXTURE0);
        glBindSampler (0, GL_LINEAR);

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        renderer.state().changeCamera (camera);
        sceneObj2->draw (renderer);
        //sceneObj1->draw (renderer);

        //dt->saveToFile ("test-depth.jpg");
        //while (!sf::Keyboard::isKeyPressed (sf::Keyboard::Space));


        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

        window.sfmlWindow().resetGLStates();
        window.sfmlWindow().draw (text);
    });


    debug::log::println ("===================================== loading finished ============================================");
    window.runEventLoop();
    debug::log::println ("==================================== rendering finished ===========================================");

    return 0;
}
