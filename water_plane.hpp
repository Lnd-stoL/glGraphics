
#ifndef __water_plane__included__
#define __water_plane__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "mesh.hpp"
#include "renderable.hpp"
#include "render_resources.hpp"
#include "scene.hpp"

using namespace render;

using math3D::transform_d;
using math3D::vector3_d;
using math3D::object2screen_transform_d;
using math3D::perspective_projection_d;

//----------------------------------------------------------------------------------------------------------------------

class water_plane : public renderable
{
    mesh_component<elementary_shapes::simple_vertex, unsigned short>::ptr  _mesh;
    transform_d  _transform;
    float _surfaceHeight;
    material::ptr  _material;

    frame_buffer::ptr  _reflectionsFrameBuffer;
    texture::ptr       _reflectionsTexture;
    camera::ptr        _reflectionsCamera;

public:
    property_get (Mesh, _mesh)
    property_get_ref (Transform, _transform)


public:
    declare_ptr_alloc (water_plane)
    water_plane (resources& renderRes, render_window &renderWindow, float y);

    void useRefractionTextures (texture::ptr refractTexture, texture::ptr depthTexture);
    void drawReflections (graphics_renderer &renderer, scene &reflectibleScene);
    virtual void draw (graphics_renderer &renderer) const;
};

//----------------------------------------------------------------------------------------------------------------------

#endif
