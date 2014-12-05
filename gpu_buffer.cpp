
#include "gpu_buffer.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    const gpu_buffer::preferred_access_t gpu_buffer::fastGPU_Draw  = 1;
    const gpu_buffer::preferred_access_t gpu_buffer::fastCPU_Read  = 2;
    const gpu_buffer::preferred_access_t gpu_buffer::compromiss    = 3;

    const gpu_buffer::change_rate_t      gpu_buffer::everyFrame    = 10;
    const gpu_buffer::change_rate_t      gpu_buffer::dynamicChange = 11;
    const gpu_buffer::change_rate_t      gpu_buffer::staticData    = 12;

//----------------------------------------------------------------------------------------------------------------------

    gl_bindable_impl (gl_vertex_buffer)
    gl_bindable_impl (gl_index_buffer )

//----------------------------------------------------------------------------------------------------------------------

    gpu_buffer::gpu_buffer (GLenum target, unsigned size, unsigned short bytesPerElement) : _size (size),
                                                                                            _target (target),
                                                                                            _bytesPerElement (bytesPerElement)
    {
        glGenBuffers (1, &_bufferId);
        if (!debug::gl::test()) return;
        debug::log::println_gl (mkstr ("created ", asString()));
    }


    gpu_buffer::~gpu_buffer()
    {
        if (_bufferId)
        {
            glDeleteBuffers (1, &_bufferId);
            debug::log::println_gl (mkstr (asString(), " released"));
        }

        _bufferId = 0;
    }


    /*virtual*/ void gpu_buffer::use() const
    {
        glBindBuffer (_target, _bufferId);
    }


    GLenum gpu_buffer::_bufferUsage (gpu_buffer::preferred_access_t preferredAccess, gpu_buffer::change_rate_t changeRate)
    {
        GLenum bufferUsage = GL_INVALID_ENUM;

        if (changeRate == everyFrame    && preferredAccess == fastGPU_Draw)  bufferUsage = GL_STREAM_DRAW;
        if (changeRate == everyFrame    && preferredAccess == fastCPU_Read)  bufferUsage = GL_STREAM_READ;
        if (changeRate == everyFrame    && preferredAccess == compromiss)    bufferUsage = GL_STREAM_COPY;

        if (changeRate == dynamicChange && preferredAccess == fastGPU_Draw)  bufferUsage = GL_DYNAMIC_DRAW;
        if (changeRate == dynamicChange && preferredAccess == fastCPU_Read)  bufferUsage = GL_DYNAMIC_READ;
        if (changeRate == dynamicChange && preferredAccess == compromiss)    bufferUsage = GL_DYNAMIC_COPY;

        if (changeRate == staticData    && preferredAccess == fastGPU_Draw)  bufferUsage = GL_STATIC_DRAW;
        if (changeRate == staticData    && preferredAccess == fastCPU_Read)  bufferUsage = GL_STATIC_READ;
        if (changeRate == staticData    && preferredAccess == compromiss)    bufferUsage = GL_STATIC_COPY;

        return bufferUsage;
    }


    /*virtual*/ string gpu_buffer::asString() const
    {
        return mkstr ("gpu_buffer [with id ", _bufferId, " of size ", _size, " (", sizeInBytes(), " bytes)]");
    }
}
