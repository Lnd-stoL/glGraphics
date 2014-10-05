
#ifndef __vertex_buffer__included__
#define __vertex_buffer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

#include <GL/glew.h>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class i_vertex_layout
    {
    public:
        struct attribute
        {
            enum type_t
            {
                tFloat  = GL_FLOAT,
                tInt    = GL_INT,
                tDouble = GL_DOUBLE
            };

            string   name;
            type_t   type;
            size_t   offset;
            unsigned dimension;
            bool     normalized;

            attribute (string name, type_t type, size_t offset, unsigned dimension = 1, bool normalized = false) :
                name       (name),
                type       (type),
                offset     (offset),
                dimension  (dimension),
                normalized (normalized)
            { }
        };

    protected:
        virtual void _registerAttributes() = 0;

    public:
        virtual const std::vector<attribute>& getAttributes() const = 0;
        virtual size_t getStride() const = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t>
    class vertex_layout :
        public i_vertex_layout
    {
    protected:
        std::vector<attribute> _attributes;

    protected:
        void _registerAttribute (string name, attribute::type_t type, unsigned offset, unsigned dimension = 1, bool normalized = false)
        {
            _attributes.push_back (attribute (name, type, offset, dimension, normalized));
        }


    public:
        property_get_ref (Attributes, _attributes)

        virtual size_t getStride() const
        {
            return sizeof (vertex_t);
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename element_t>
    class gpu_buffer :
        public oo_extensions::i_as_string,
        public oo_extensions::non_copyable
    {
    protected:
        GLuint _bufferId = GL_INVALID_INDEX;
        unsigned _size   = 0;
        GLenum _target   = GL_INVALID_ENUM;

    public:
        typedef int preferred_access_t;
        static constexpr preferred_access_t fastGPU_Draw  = 1;
        static constexpr preferred_access_t fastCPU_Read  = 2;
        static constexpr preferred_access_t compromiss    = 3;

        typedef int change_rate_t;
        static constexpr change_rate_t      everyFrame    = 10;
        static constexpr change_rate_t      dynamicChange = 11;
        static constexpr change_rate_t      staticData    = 12;

        typedef int mapping_access_t;
        static constexpr mapping_access_t   accessReadOnly  = GL_READ_ONLY;
        static constexpr mapping_access_t   accessWriteOnly = GL_WRITE_ONLY;
        static constexpr mapping_access_t   accessReadWrite = GL_READ_WRITE;


    public:
        property_get (GlId,        _bufferId)
        property_get (SizeInBytes, _size * sizeof (element_t))
        property_get (Size,        _size)

    public:
        gpu_buffer (GLenum target, const element_t *data, unsigned size, preferred_access_t preferredAccess, change_rate_t changeRate);
        gpu_buffer (GLenum target, const std::vector<element_t> &data, preferred_access_t preferredAccess, change_rate_t changeRate);

        virtual std::string asString() const;
        void use() const;

        ~gpu_buffer();
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t>
    class vertex_buffer : public gpu_buffer<vertex_t>
    {
    protected:
        typedef gpu_buffer<vertex_t> base_t;

    public:
        vertex_buffer (vertex_t *vertices, unsigned vertexCount,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer (GL_ARRAY_BUFFER, vertices, vertexCount, preferredAccess, changeRate) { }

        vertex_buffer (const std::vector<vertex_t> &data,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer (GL_ARRAY_BUFFER, data, preferredAccess, changeRate) { }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename index_t>
    class index_buffer : public gpu_buffer<index_t>
    {
    protected:
        typedef gpu_buffer<index_t> base_t;

    public:
        index_buffer (index_t *indices, unsigned indexCount,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer (GL_ELEMENT_ARRAY_BUFFER, indices, indexCount, preferredAccess, changeRate) { }

        index_buffer (const std::vector<index_t> &data,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer (GL_ELEMENT_ARRAY_BUFFER, data, preferredAccess, changeRate) { }
    };
}

//----------------------------------------------------------------------------------------------------------------------

#include "vertex_buffer.cpp"

#endif
