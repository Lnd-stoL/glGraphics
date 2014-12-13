
#ifndef __shadowmaps_shadows__included__
#define __shadowmaps_shadows__included__

//----------------------------------------------------------------------------------------------------------------------

#include "renderable.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "frame_buffer.hpp"
#include "scene.hpp"
#include "graphics_renderer.hpp"
#include "gpu_image_processing.hpp"
#include "exs3d.hpp"

using math3d::vector2;
using math3d::rotation_d;
using math3d::vector3_d;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class shadowmap_shadows
    {
        camera::ptr  _lightCamera;
        offscreen_render_target::ptr  _shadowmapRT;
        material::ptr  _shadowmapGenMaterial;
        matrix_4x4_f _matBias;


    public:
        declare_ptr_alloc (shadowmap_shadows)
        shadowmap_shadows (resources &renderRes, vector2<unsigned> mapSize);

        void generateShadowmap (graphics_renderer &renderer, scene::ptr fromScene, scene::render_group_id shadowCasters);
        void drawShadedScene (graphics_renderer &renderer, scene::ptr fromScene, scene::render_group_id shadowRecv);

        void changeLight (const vector3_d &position, const rotation_d &rotation);
    };

//----------------------------------------------------------------------------------------------------------------------

    struct shadowmapgen_vertex_layout : vertex_layout<exs3d_mesh::vertex>
    {
    protected:
        virtual void _registerAttributes()
        {
            _attributes.clear();
            _registerAttribute ("iCoords", attribute::tFloat, offsetof (exs3d_mesh::vertex, coords), 3);
        }

    public:
        declare_ptr_alloc (shadowmapgen_vertex_layout)
        shadowmapgen_vertex_layout()  { _registerAttributes(); }
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
