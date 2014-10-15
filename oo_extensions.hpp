
#ifndef __oo_extensions__included__
#define __oo_extensions__included__

//----------------------------------------------------------------------------------------------------------------------

#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

using std::shared_ptr;
using std::vector;
using std::string;
using std::make_shared;

//----------------------------------------------------------------------------------------------------------------------

#define property_get(public_name, private_name)              \
    inline decltype(private_name) get##public_name() const   \
    {                                                        \
        return private_name;                                 \
    }


#define property_get_ref(public_name, private_name)          \
    inline const decltype(private_name)& get##public_name() const   \
    {                                                        \
        return private_name;                                 \
    }


#define property_set(public_name, private_name)                 \
    inline void set##public_name(decltype(private_name) value)  \
    {                                                           \
        private_name = value;                                   \
    }


#define property_rw(public_name, private_name)  \
    property_set(public_name, private_name)     \
    property_get(public_name, private_name)


#define prevent_copying(class_name)             \
    protected: class_name (const class_name &); \
    protected: class_name (const class_name &&);

//----------------------------------------------------------------------------------------------------------------------

namespace oo_extensions
{
    class non_copyable
    {
        prevent_copying (non_copyable)

    public:
        non_copyable() {  }
    };

//----------------------------------------------------------------------------------------------------------------------

    class i_as_string
    {
    public:
        virtual std::string asString() const = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename T1>
    inline string mkstr (T1 x1)
        { return ((std::stringstream &)(std::stringstream() << x1)).str(); }

    template<typename T1, typename T2>
    inline string mkstr (T1 x1, T2 x2)
        { return ((std::stringstream &)(std::stringstream() << x1 << x2)).str(); }

    template<typename T1, typename T2, typename T3>
    inline string mkstr (T1 x1, T2 x2, T3 x3)
        { return ((std::stringstream &)(std::stringstream() << x1 << x2 << x3)).str(); }

    template<typename T1, typename T2, typename T3, typename T4>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4)
        { return ((std::stringstream &)(std::stringstream() << x1 << x2 << x3 << x4)).str(); }

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5)
        { return ((std::stringstream &)(std::stringstream() << x1 << x2 << x3 << x4 << x5)).str(); }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6)
        { return ((std::stringstream &)(std::stringstream() << x1 << x2 << x3 << x4 << x5 << x6)).str(); }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7)
        { return ((std::stringstream &)(std::stringstream() << x1 << x2 << x3 << x4 << x5 << x6 << x7)).str(); }


//----------------------------------------------------------------------------------------------------------------------

    template<typename... handler_args_t>
    class event
    {
    public:
        typedef std::function<void(handler_args_t...)> handler_t;

    protected:
        vector<handler_t> _handlers;

    public:
        inline void fire (const handler_args_t&... arguments)
        {
            for (auto nextHandler : _handlers)  nextHandler (arguments...);
        }

        inline void operator() (const handler_args_t&... arguments)
        {
            fire (arguments...);
        }

        inline void handleWith (handler_t handler)
        {
            _handlers.push_back (handler);
        }

        inline void stopHandleWith (handler_t handler)
        {
            _handlers.erase (_handlers.find (handler));
        }
    };


#define event_access(public_name, private_name)                       \
    inline decltype(private_name)& public_name##Event()               \
    {                                                                 \
        return private_name;                                          \
    }

//----------------------------------------------------------------------------------------------------------------------

    #define declare_ptr(object_t)          typedef shared_ptr<object_t> ptr;

    #define declare_ptr_alloc(object_t)    declare_ptr (object_t)                                   \
                                           template<typename... args_t>                             \
                                           static typename object_t::ptr alloc (args_t... ctorArgs) \
                                           { return make_shared<object_t> (ctorArgs...); }

}

//----------------------------------------------------------------------------------------------------------------------

#endif
