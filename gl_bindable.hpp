
#ifndef __gl_bindable__included__
#define __gl_bindable__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    template<typename class_t>
    class gl_bindable
    {
    private:
        static const gl_bindable* _boundNowInGL;

    protected:
        void _bindThis() const
        {
            _boundNowInGL = this;
        }

        static void _bindDefault()
        {
            _boundNowInGL = nullptr;
        }


    public:
        declare_ptr (gl_bindable)

        bool isBoundNow() const               { return _boundNowInGL == this; }
        static const gl_bindable* boundNow()  { return _boundNowInGL; }
        static bool isDefault()               { return _boundNowInGL = nullptr; }
    };

//----------------------------------------------------------------------------------------------------------------------

    #define gl_bindable_impl(class_t)    template<> const gl_bindable<class_t>* gl_bindable<class_t>::_boundNowInGL = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

#endif
