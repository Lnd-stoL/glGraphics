
#ifndef __dynamic_sky__included__
#define __dynamic_sky__included__

//----------------------------------------------------------------------------------------------------------------------

#include "mesh.hpp"
#include "render_resources.hpp"
#include "scene.hpp"
#include "SFML/Graphics.hpp"

using namespace render;
using math3d::vector3_f;

//----------------------------------------------------------------------------------------------------------------------

class dynamic_sky : public renderable
{
    mesh::ptr  _mesh;
    material::ptr  _material;
    sf::Image  _horizonColorMap;

    float _skySphereRadius = 0;

    double _dayTime = 0;
    double _timeMultiplyer = 0.1;
    color_rgb<float>  _sunColor;
    vector3_f  _sunDir;
    bool  _timePaused = false;


public:
    property_rw (skySphereRadius, _skySphereRadius)
    property_rw (timeMultiplyer, _timeMultiplyer)

    property_get (dayTime, _dayTime)
    property_get (sunColor, _sunColor)
    property_get (sunDirection, _sunDir)
    property_get (timePaused, _timePaused)


public:
    declare_ptr_alloc (dynamic_sky)
    dynamic_sky (resources& renderRes, float skySphereRad);

    void update (double time);
    virtual void draw (graphics_renderer &renderer) const;

    void timePause();
    void timeContinue();
};

//----------------------------------------------------------------------------------------------------------------------

#endif
