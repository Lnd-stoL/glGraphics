
#include "binary_stream.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace utils
{
    binary_reader::binary_reader (shared_ptr<std::istream> stream) : _stream (stream)
    {

    }


    void binary_reader::read (unsigned char *buffer, unsigned length)
    {
        _stream->read ((char *)buffer, length);
    }


    unsigned char binary_reader::readByte()
    {
        unsigned char data;
        this->read (&data, sizeof (unsigned char));
        return data;
    }


    string binary_reader::readString (unsigned length)
    {
        char str[length+1];
        this->read ((unsigned char *)str, length);
        str[length] = 0;
        return string (str);
    }


    string binary_reader::readShortString()
    {
        unsigned char strLen = this->readByte();
        return this->readString (strLen);
    }


    u_int32_t binary_reader::readUInt32()
    {
        u_int32_t data;
        this->read ((unsigned char *)&data, sizeof (u_int32_t));
        return data;
    }
}
