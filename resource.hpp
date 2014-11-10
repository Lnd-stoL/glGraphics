
#ifndef __resource_included__
#define __resource_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"

//----------------------------------------------------------------------------------------------------------------------

class resource :
    public oo_extensions::non_copyable,
    public oo_extensions::i_as_string
{
protected:
    bool _wasChangedAfterLoading = true;
    void _changedAfterLoading()  { _wasChangedAfterLoading = true;  }
    void _loaded()               { _wasChangedAfterLoading = false; }

public:
    struct id
    {
        virtual string hashString() const = 0;
    };


public:
    declare_ptr (resource)

    virtual ~resource()  { };
    virtual string asString() const  { return "<resource>"; }

    virtual vector<string> supportedFileExtensions() const  { return vector<string>(); };
    bool isValidAsResource()  { return !_wasChangedAfterLoading; }
};

//----------------------------------------------------------------------------------------------------------------------

#endif
