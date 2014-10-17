
#ifndef __resource_included__
#define __resource_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"

//----------------------------------------------------------------------------------------------------------------------

class resource :
    public oo_extensions::non_copyable,
    public oo_extensions::i_as_string
{
public:
    class identifyer
    {
    public:
        virtual string hashString() const = 0;
    };


public:
    declare_ptr (resource)
    virtual string asString() const  { return "<resource>"; }
};

//----------------------------------------------------------------------------------------------------------------------

#endif
