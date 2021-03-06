#ifndef __water_plane__included__
#define __water_plane__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "render_resources.hpp"
#include "graphics_renderer.hpp"
#include "gpu_image_processing.hpp"

using namespace render;

//----------------------------------------------------------------------------------------------------------------------

class water_plane : public renderable
{
    mesh_component<elementary_shapes::simple_vertex, unsigned short>::ptr  _mesh;
    float _surfaceHeight;
    material::ptr  _material;

    offscreen_render_target::ptr  _reflectionsRT;
    texture::ptr                  _reflectionsTexture;
    camera::ptr                   _reflectionsCamera;

public:
    property_get (surfaceHeight, _surfaceHeight)


public:
    declare_ptr_alloc (water_plane)
    water_plane (resources& renderRes, render_window &renderWindow, float y);

    void useRefractionTextures (texture::ptr refractTexture, texture::ptr depthTexture);
    void drawReflections (graphics_renderer &renderer, scene &reflectibleScene, scene::render_group_id reflectibleGroup);

    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
