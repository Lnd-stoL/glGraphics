
#ifndef __gpu_buffer_impl__included__
#define __gpu_buffer_impl__included__

//----------------------------------------------------------------------------------------------------------------------

#include "gpu_buffer.hpp"
#include "debug.hpp"

#include <fstream>
#include <exception>
#include <stdexcept>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename element_t>
    gpu_buffer_of<element_t>::gpu_buffer_of (GLenum target, const element_t* data, unsigned size,
                                             preferred_access_t preferredAccess,
                                             change_rate_t changeRate)
        : gpu_buffer (target, size, (unsigned short) sizeof (element_t))
    {
        _fillData ((void *)data, target, preferredAccess, changeRate);
    }


    template<typename element_t>
    gpu_buffer_of<element_t>::gpu_buffer_of (GLenum target, const std::vector<element_t> &data,
                                             preferred_access_t preferredAccess,
                                             change_rate_t changeRate)
        : gpu_buffer (target, data.size(), (unsigned short) sizeof (element_t))
    {
        _fillData ((void *)data.data(), target, preferredAccess, changeRate);
    }


    template<typename element_t>
    void gpu_buffer_of<element_t>::_fillData (void *data, GLenum target, preferred_access_t preferredAccess,
                                              change_rate_t changeRate)
    {
        use();
        glBufferData (target, getSizeInBytes(), data, _bufferUsage (preferredAccess, changeRate));
        debug::gl::test();
    }

//----------------------------------------------------------------------------------------------------------------------

    template<typename vertex_t>
    /*virtual*/ void vertex_buffer<vertex_t>::use() const
    {
        if (gl_vertex_buffer::isBoundNow())  return;
        gpu_buffer::use();
        gl_vertex_buffer::_bindThis();
    }


    template<typename index_t>
    /*virtual*/ void index_buffer<index_t>::use() const
    {
        if (gl_index_buffer::isBoundNow())  return;
        gpu_buffer::use();
        gl_index_buffer::_bindThis();
    }
}

//----------------------------------------------------------------------------------------------------------------------

#endif
