
#include "gpu_buffer.hpp"
#include "debug.hpp"

#include <GL/glew.h>
#include <fstream>
#include <exception>
#include <stdexcept>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename element_t>
    gpu_buffer<element_t>::gpu_buffer (GLenum target, const element_t* data, unsigned size,
                                       preferred_access_t preferredAccess, change_rate_t changeRate) : _size (size),
                                                                                                       _target (target)
    {
        glGenBuffers (1, &_bufferId);
        if (!debug::gl::test()) return;
        debug::log::println_gl (mkstr ("created ", asString()));

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

        use();
        glBufferData (target, getSizeInBytes(), data, bufferUsage);
        debug::gl::test();
    }


    template<typename element_t>
    gpu_buffer<element_t>::gpu_buffer (GLenum bufferUsage, const std::vector<element_t> &data,
                                       preferred_access_t preferredAccess, change_rate_t changeRate) :
                                       gpu_buffer (bufferUsage, data.data(), data.size(), preferredAccess, changeRate)
    {  }


    template<typename element_t>
    gpu_buffer<element_t>::~gpu_buffer()
    {
        if (_bufferId && _bufferId != GL_INVALID_VALUE)
        {
            glDeleteBuffers (1, &_bufferId);
            debug::log::println_gl (mkstr (asString(), " released"));
        }

        _bufferId = GL_INVALID_VALUE;
    }


    template<typename element_t>
    /*virtual*/ std::string gpu_buffer<element_t>::asString() const
    {
        return mkstr ("gpu_buffer [with id ", _bufferId, " of size ", _size, " (", getSizeInBytes(), " bytes)]");
    }


    template<typename element_t>
    void gpu_buffer<element_t>::use() const
    {
        glBindBuffer (_target, _bufferId);
        debug::gl::test();
    }

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t>
    void vertex_buffer<vertex_t>::use() const
    {
        if (gl_vertex_buffer::isBoundNow())  return;
        gpu_buffer<vertex_t>::use();
        gl_vertex_buffer::_bindThis();
    }


    template<typename index_t>
    void index_buffer<index_t>::use() const
    {
        if (gl_index_buffer::isBoundNow())  return;
        gpu_buffer<index_t>::use();
        gl_index_buffer::_bindThis();
    }
}
