
#ifndef __screen_overlay__included__
#define __screen_overlay__included__

//----------------------------------------------------------------------------------------------------------------------

#include "math3D.hpp"
#include "oo_extensions.hpp"
#include "gl_binding.hpp"
#include "font_resource.hpp"
#include "renderable.hpp"
#include "mesh.hpp"
#include "render_resources.hpp"

using math3d::vector2_d;
using math3d::vector2_f;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class screen_overlay_layer;

//----------------------------------------------------------------------------------------------------------------------

    class screen_overlay :
        public oo_extensions::non_copyable
    {
    protected:
        vector2_f  _position;
        vector2_f  _size;

    public:
        property_rw (ScreenPosition, _position);
        property_rw (ScreenSize, _size);


    public:
        declare_ptr (screen_overlay)
        screen_overlay (vector2_f position, vector2_f size);

        virtual void draw (const screen_overlay_layer& overlayLayer, graphics_renderer& renderer) = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    class text_renderer :
        public oo_extensions::non_copyable
    {
    public:
        static const unsigned maxTextLineLength = 1024;

    public:
        struct text_vertex
        {
            vector2_f  position;
            vector2_f  texUV;
        };


        struct text_vertex_layout : vertex_layout<text_vertex>
        {
        protected:
            virtual void _registerAttributes()
            {
                _registerAttribute ("aCoords", attribute::tFloat, offsetof (text_vertex, position), 2);
                _registerAttribute ("aTexUV",  attribute::tFloat, offsetof (text_vertex, texUV),    2);
            }

        public:
            declare_ptr_alloc (text_vertex_layout)
            text_vertex_layout()  { _registerAttributes(); }
        };


    private:
        FT_Library _freeType;

        technique::ptr  _technique;
        vector<text_vertex>  _vertices;
        index_buffer<uint16_t>::ptr  _indices;


    protected:
        void _generateVertices();
        void _generateIndices();

    public:
        property_get (TextRenderingTechnique, _technique)
        property_get (TextLineVertices, _vertices);
        property_get (TextLineIndexBuffer, _indices);

    public:
        declare_ptr_alloc (text_renderer)

        text_renderer (resources &renderRes);
    };

//----------------------------------------------------------------------------------------------------------------------

    class screen_overlay_layer :
            public renderable,
            public oo_extensions::non_copyable
    {
        text_renderer::ptr  _textRenderer;
        vector<screen_overlay::ptr>  _overlays;

    public:
        property_get (TextRenderer, _textRenderer);

    public:
        declare_ptr_alloc (screen_overlay_layer)
        screen_overlay_layer (resources &renderRes);

        void addOverlay (screen_overlay::ptr overlay);
        virtual void draw (graphics_renderer &renderer) const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class text_label :
        public screen_overlay,
        public oo_extensions::i_as_string
    {
    private:
        font::ptr  _font;
        string  _text;
        color_rgb<float>  _color = color_rgb<float> (1, 1, 1);

        bool _meshUpdateNeeded = true;
        material::ptr _material;
        vertex_buffer<text_renderer::text_vertex>::ptr  _textMeshVB;
        unsigned _vBuffCapacity = 0;


    public:
        property_get_ref (Text, _text)
        property_get (Font, _font)
        property_rw (Color, _color);


    protected:
        void _loadMaterial (text_renderer::ptr textRenderer);
        void _updateTextMesh (text_renderer::ptr textRenderer);
        void _vBuffNeeded (text_renderer::ptr textRenderer);

    public:
        declare_ptr_alloc (text_label)

        text_label (font::ptr fnt, vector2_f initialPos, vector2_f size, string initialText = "");
        virtual void draw (const screen_overlay_layer& overlayLayer, graphics_renderer& renderer);
        void changeText (const string &txt);
        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
