
#ifndef __vertex_buffer__included__
#define __vertex_buffer__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "gl_bindable.hpp"
#include "gl_binding.hpp"

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
                tFloat  = (unsigned) GL_FLOAT,
                tInt    = (unsigned) GL_INT,
                tDouble = (unsigned) GL_DOUBLE
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

            string hashString() const
            {
                return mkstr (name, '#', type, '#', offset, '#', dimension, '#', normalized);
            }
        };


    protected:
        std::vector<attribute> _attributes;

    protected:
        void _registerAttribute (string name, attribute::type_t type, unsigned offset, unsigned dimension = 1, bool normalized = false)
        {
            _attributes.push_back (attribute (name, type, offset, dimension, normalized));
        }


    public:
        declare_ptr (i_vertex_layout)
        property_get_ref (Attributes, _attributes)
        virtual size_t getStrideInBytes() const = 0;

        string hashString() const
        {
            std::stringstream tmp;
            for (auto nextAttribute : getAttributes())  tmp << nextAttribute.hashString() << '*';
            return tmp.str();
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t>
    class vertex_layout :
        public i_vertex_layout
    {
    public:
        declare_ptr (vertex_layout)

        virtual size_t getStrideInBytes() const
        {
            return sizeof (vertex_t);
        }
    };


//----------------------------------------------------------------------------------------------------------------------

    class dynamic_vertex_layout :
        public i_vertex_layout
    {
    protected:
        size_t _vertexSizeInBytes = 0;

    public:
        declare_ptr (dynamic_vertex_layout)

        dynamic_vertex_layout (size_t vertexSizeInBytes) : _vertexSizeInBytes (vertexSizeInBytes)
        { }

        virtual size_t getStrideInBytes() const  { return _vertexSizeInBytes; };
    };

//----------------------------------------------------------------------------------------------------------------------

    class gpu_buffer :
        public oo_extensions::non_copyable,
        public oo_extensions::i_as_string
    {
    protected:
        GLuint _bufferId = GL_INVALID_INDEX;
        unsigned _size   = 0;
        unsigned short _bytesPerElement = 0;
        GLenum _target   = GL_INVALID_ENUM;

    public:
        typedef uint8_t preferred_access_t;
        static const preferred_access_t fastGPU_Draw;
        static const preferred_access_t fastCPU_Read;
        static const preferred_access_t compromiss;

        typedef uint8_t change_rate_t;
        static const change_rate_t      everyFrame;
        static const change_rate_t      dynamicChange;
        static const change_rate_t      staticData;

        typedef int mapping_access_t;
        static const mapping_access_t   accessReadOnly  = (mapping_access_t) GL_READ_ONLY;
        static const mapping_access_t   accessWriteOnly = (mapping_access_t) GL_WRITE_ONLY;
        static const mapping_access_t   accessReadWrite = (mapping_access_t) GL_READ_WRITE;


    public:
        property_get (GlId,  _bufferId)
        property_get (Size,  _size)
        property_get (SizeInBytes, _size * _bytesPerElement)


    protected:
        GLenum _bufferUsage (preferred_access_t preferredAccess, change_rate_t changeRate);
        virtual void _fillData (void *data, GLenum target, preferred_access_t preferredAccess,
                                change_rate_t changeRate) = 0;

    public:
        declare_ptr (gpu_buffer)
        gpu_buffer (GLenum target, unsigned size, unsigned short bytesPerElement);

        virtual void use() const;

        virtual string asString() const;
        virtual ~gpu_buffer();
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename element_t>
    class gpu_buffer_of : public gpu_buffer
    {
    protected:
        virtual void _fillData (void *data, GLenum target, preferred_access_t preferredAccess,
                                change_rate_t changeRate);

    public:
        declare_ptr (gpu_buffer_of)

        gpu_buffer_of (GLenum target, const element_t *data, unsigned size, preferred_access_t preferredAccess,
                       change_rate_t changeRate);

        gpu_buffer_of (GLenum target, const std::vector<element_t> &data, preferred_access_t preferredAccess,
                       change_rate_t changeRate);

        element_t*cpuAccess (mapping_access_t access);
        void gpuAccess ();
    };

//----------------------------------------------------------------------------------------------------------------------

    class gl_vertex_buffer : public gl_bindable<gl_vertex_buffer>
    {  };


    template<typename vertex_t>
    class vertex_buffer :
        public gpu_buffer_of<vertex_t>,
        public gl_vertex_buffer
    {
    protected:
        typedef gpu_buffer_of<vertex_t> base_t;

    public:
        typedef vertex_buffer<vertex_t> this_t;
        declare_ptr_alloc (this_t)

        vertex_buffer (vertex_t *vertices, unsigned vertexCount,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer_of (GL_ARRAY_BUFFER, vertices, vertexCount, preferredAccess, changeRate) { }

        vertex_buffer (const std::vector<vertex_t> &data,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer_of (GL_ARRAY_BUFFER, data, preferredAccess, changeRate) { }

        virtual void use() const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class gl_index_buffer : public gl_bindable<gl_index_buffer>
    {  };


    template<typename index_t>
    class index_buffer :
        public gpu_buffer_of<index_t>,
        public gl_index_buffer
    {
    protected:
        typedef gpu_buffer_of<index_t> base_t;

    public:
        typedef index_buffer<index_t> this_t;
        declare_ptr_alloc (this_t)

        index_buffer (index_t *indices, unsigned indexCount,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer_of (GL_ELEMENT_ARRAY_BUFFER, indices, indexCount, preferredAccess, changeRate) { }

        index_buffer (const std::vector<index_t> &data,
            typename base_t::preferred_access_t preferredAccess = base_t::fastGPU_Draw,
            typename base_t::change_rate_t changeRate = base_t::staticData) :
                base_t::gpu_buffer_of (GL_ELEMENT_ARRAY_BUFFER, data, preferredAccess, changeRate) { }

        virtual void use() const;
        uint8_t bytesPerIndex() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif

