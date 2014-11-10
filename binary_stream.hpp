
#ifndef __binary_stream__included__
#define __binary_stream__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace utils
{
    class binary_reader
    {
        shared_ptr<std::istream> _stream;

    public:
        binary_reader (shared_ptr<std::istream> stream);

        void read (unsigned char *buffer, unsigned length);
        unsigned char readByte();
        u_int32_t readUInt32();

        string readString (unsigned length);
        string readShortString();

        template<typename element_t>
        vector<element_t> readArrayOf();
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
