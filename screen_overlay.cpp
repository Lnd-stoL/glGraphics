
#include "screen_overlay.hpp"

#include "debug.hpp"
#include "graphics_renderer.hpp"
#include "gpu_buffer_impl.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    text_label::text_label (font::ptr fnt, vector2_f initialPos, vector2_f size, string initialText) :
        screen_overlay (initialPos, size),
        _font (fnt),
        _text (initialText)
    {
    }


    /*virtual*/ string text_label::asString() const
    {
        return _text;
    }


    /*virtual*/ void text_label::draw (const screen_overlay_layer &overlayLayer, graphics_renderer& renderer)
    {
        if (!_visible)  return;

        auto textRenderer = overlayLayer.textRenderer();
        if (!_material)         _loadMaterial (textRenderer);
        if (_meshUpdateNeeded)  _updateTextMesh (textRenderer);

        _material->vec2Params()["uOffset"] = vector2_f (_position.x() * 2 - 1, 1 - _position.y() * 2);
        _material->vec2Params()["uScale"]  = _size;
        _material->vec3Params()["uColor"]  = _color.asVector();

        renderer.use (_material);
        auto pIndexBuffer = textRenderer->textLineIndexBuffer();
        renderer.draw (*_textMeshVB, *pIndexBuffer, sizeof (uint16_t), (unsigned) _text.size() * 6);
    }


    void text_label::_vBuffNeeded (text_renderer::ptr textRenderer)
    {
        if (_text.length() <= _vBuffCapacity)  return;

        _vBuffCapacity = (unsigned) std::max (_vBuffCapacity * 1.5 + 1, (double) _text.length());
        vector<text_renderer::text_vertex>  vertices = textRenderer->textLineVertices();
        if (_vBuffCapacity > vertices.size())  _vBuffCapacity = (unsigned) vertices.size();
        vertices.resize (_vBuffCapacity * 4);

        _textMeshVB = vertex_buffer<text_renderer::text_vertex>::alloc (vertices,
                                                                        gpu_buffer::fastGPU_Draw,
                                                                        gpu_buffer::dynamicChange);
    }


    void text_label::_loadMaterial (text_renderer::ptr textRenderer)
    {
        _material = material::alloc (textRenderer->textRenderingTechnique());
        _material->textures()["uFontBitmap"] = _font->bitmap();
    }


    void text_label::_updateTextMesh (text_renderer::ptr textRenderer)
    {
        _vBuffNeeded (textRenderer);
        text_renderer::text_vertex *vBuffData = _textMeshVB->cpuAccess (gpu_buffer::accessReadWrite);

        static const vector2_f texCoordsSrc[4] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
        vector2_f texUVScaling (1.0f / 16.0f, 1.0f / 16.0f);
        for (unsigned i = 0; i < _text.length(); ++i)
        {
            unsigned offset = i * 4;
            float letterIndexX = _text[i] % 16;
            float letterIndexY = _text[i] / 16;

            for (unsigned j = 0; j < 4; ++j)
            {
                vector2_f& texUV = vBuffData[offset + j].texUV;
                texUV = texCoordsSrc[j];
                texUV.scale (vector2_f (0.6, 1)) += vector2_f (0.2, 0);
                texUV += vector2_f (letterIndexX, letterIndexY);
                texUV.scale (texUVScaling);
            }
        }

        _textMeshVB->gpuAccess();
        _meshUpdateNeeded = false;
    }


    void text_label::changeText (const string &txt)
    {
        _text = txt;
        _meshUpdateNeeded = true;
    }

//----------------------------------------------------------------------------------------------------------------------

    screen_overlay::screen_overlay (vector2_f position, vector2_f size) :
        _position (position),
        _size (size)
    {

    }


    screen_overlay_layer::screen_overlay_layer (resources &renderRes)
    {
        _textRenderer = text_renderer::alloc (renderRes);
    }


    void screen_overlay_layer::addOverlay (screen_overlay::ptr overlay)
    {
        _overlays.push_back (overlay);
    }


    void screen_overlay_layer::draw (graphics_renderer &renderer) const
    {
        if (!_visible)  return;

        glDisable (GL_CULL_FACE);
        glDisable (GL_DEPTH_TEST);
        glDepthMask (GL_FALSE);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto nextOverlay : _overlays)
        {
            if (nextOverlay->isVisible())
            {
                nextOverlay->draw (*this, renderer);
            }
        }

        glDisable (GL_BLEND);
        glDepthMask (GL_TRUE);
        glEnable (GL_CULL_FACE);
        glEnable (GL_DEPTH_TEST);
    }

//----------------------------------------------------------------------------------------------------------------------

    text_renderer::text_renderer (resources &renderRes)
    {
        auto programId = gpu_program::id (text_vertex_layout::alloc(), "text_line.vert", "text_line.frag");
        _technique = technique::alloc (renderRes.gpuProgramsManager().request (programId, renderRes));
        _technique->transformNotNeeded();

        _generateVertices();
        _generateIndices();

        //FT_Init_FreeType (&_freeType);
    }


    void text_renderer::_generateVertices()
    {
        _vertices.clear();
        _vertices.reserve (maxTextLineLength * 4);

        for (float i = 0; i < maxTextLineLength; ++i)
        {
            const float xNarrowing = 0.6;
            float iNarrowed = xNarrowing * i;

            _vertices.emplace_back (text_vertex { {iNarrowed,               0}, {0, 0} });
            _vertices.emplace_back (text_vertex { {iNarrowed,              -1}, {0, 1} });
            _vertices.emplace_back (text_vertex { {iNarrowed + xNarrowing,  0}, {1, 0} });
            _vertices.emplace_back (text_vertex { {iNarrowed + xNarrowing, -1}, {1, 1} });
        }
    }


    void text_renderer::_generateIndices()
    {
        vector<uint16_t>  indices;
        indices.reserve (maxTextLineLength * 2 * 3);

        for (unsigned i = 0; i < maxTextLineLength; ++i)
        {
            uint16_t offset = (uint16_t)i * 4;

            indices.push_back (offset + 1);
            indices.push_back (offset + 3);
            indices.push_back (offset + 0);

            indices.push_back (offset + 0);
            indices.push_back (offset + 3);
            indices.push_back (offset + 2);
        }

        _indices = index_buffer<uint16_t>::alloc (indices);
    }
}
