
#include "dynamic_sky.hpp"

#include "config_file.hpp"
#include "clouds_noise.hpp"
#include "gpu_buffer_impl.hpp"
#include "mesh_impl.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

void dynamic_sky::draw (graphics_renderer &renderer) const
{
    renderer.use (renderer.state().activeCamera()->object2ScreenTransform (transform_d()));
    _mesh->draw (renderer);
}


dynamic_sky::dynamic_sky (resources& renderRes, float skySphereRad) : _skySphereRadius (skySphereRad)
{
    auto skyboxShaderId = gpu_program::id (exs3d_mesh::exs3d_vertex_layout::alloc(), "special/sky.vert", "special/sky.frag");
    auto skyboxShader = renderRes.gpuProgramsManager().request (skyboxShaderId, renderRes);
    _material = material::alloc (technique::alloc (skyboxShader));

    _mesh = renderRes.requestFromFile<exs3d_mesh> ("sphere.exs3d")->renderableMesh();
    _mesh->components()[0]->changeMaterial (_material);
    _mesh->components()[0]->backfaceCulling (false);

    string baseFilder = "skybox/day-2/";
    std::map<string, string> cubeMapFaces;
    /*cubeMapFaces["+X"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "right.jpg"));
    cubeMapFaces["-X"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "left.jpg"));
    cubeMapFaces["+Y"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "top.jpg"));
    cubeMapFaces["-Y"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "bottom.jpg"));
    cubeMapFaces["+Z"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "front.jpg"));
    cubeMapFaces["-Z"] = renderRes.texturesManager().locateFile (mkstr (baseFilder, "back.jpg"));
    */

    vector3<unsigned> clouds3dSize (512, 512, 8);
    clouds_noise_3d  cloudsNoise (clouds3dSize);
    texture::ptr clouds3dTexture = texture::create3D (clouds3dSize, cloudsNoise.voxelRawData());
    _material->set ("uTxtClouds3D", clouds3dTexture);

    _horizonColorMap.loadFromFile (renderRes.texturesManager().locateFile ("skybox/horizon.png"));

    auto skyConfig = renderRes.requestFromFile<config_set> ("sky.info")->group ("dynamic_sky");
    _timeMultiplyer = skyConfig->get<double> ("time_multiplyer");
    _dayTime = skyConfig->get<double> ("start_time") * angle_d::pi;
}


void dynamic_sky::update (double time)
{
    if (_timePaused)  return;

    _dayTime += time * _timeMultiplyer;

    _sunDir = vector3_f (0, (float) std::sin (_dayTime), (float) std::cos (_dayTime)).normalized();

    double sunColorMapSampleX = (2.0 * _dayTime / angle_d::pi);
    if (sunColorMapSampleX >= 1.0)  sunColorMapSampleX = 2.001 - sunColorMapSampleX;
    unsigned sunColorMapCoordX = (unsigned) (sunColorMapSampleX  * _horizonColorMap.getSize().x);
    if (sunColorMapCoordX >= _horizonColorMap.getSize().x)  sunColorMapCoordX = _horizonColorMap.getSize().x - 1;

    _sunColor = color_rgb<float> (_horizonColorMap.getPixel (sunColorMapCoordX, 0));
    if (sunColorMapCoordX >= _horizonColorMap.getSize().x)  _sunColor = color_rgb<float> (0.05, 0.05, 0.05);

    _material->set ("uSunColor", _sunColor.asVector());
    _material->set ("uSunDir", _sunDir);
    _material->set ("uDayTime", (float) _dayTime);
    _material->set ("uSkySphereRadius", _skySphereRadius);
}


void dynamic_sky::timePause()
{
    _timePaused = true;
}


void dynamic_sky::timeContinue()
{
    _timePaused = false;
}
