
#include "binary_stream.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace utils
{
    template<typename element_t>
    vector<element_t> binary_reader::readArrayOf()
    {
        auto length = this->readUInt32();
        vector<element_t> array (length);
        this->read ((unsigned char *)array.data(), (unsigned) sizeof (element_t) * length);
        return std::move (array);
    }
}
