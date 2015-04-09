
#ifndef __oo_extensions__included__
#define __oo_extensions__included__

//----------------------------------------------------------------------------------------------------------------------

#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <array>

using std::shared_ptr;
using std::vector;
using std::string;
using std::make_shared;
using std::unique_ptr;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::to_string;
using std::array;

//----------------------------------------------------------------------------------------------------------------------

#define property_get(public_name, private_name)              \
    inline decltype(private_name) public_name() const        \
    {                                                        \
        return private_name;                                 \
    }


#define property_get_ref(public_name, private_name)           \
    inline const decltype(private_name)& public_name() const  \
    {                                                         \
        return private_name;                                  \
    }


#define property_ref(public_name, private_name)          \
    inline decltype(private_name)& public_name()         \
    {                                                    \
        return private_name;                             \
    }


#define property_set(public_name, private_name)                 \
    inline void public_name(decltype(private_name) value)       \
    {                                                           \
        private_name = value;                                   \
    }


#define property_set_ref(public_name, private_name)              \
    inline void public_name(decltype(private_name) &value)       \
    {                                                            \
        private_name = value;                                    \
    }


#define property_rw(public_name, private_name)  \
    property_set(public_name, private_name)     \
    property_get(public_name, private_name)


#define property_rw_ref(public_name, private_name)  \
    property_set_ref(public_name, private_name)     \
    property_get_ref(public_name, private_name)


#define prevent_copying(class_name)                       \
    protected: class_name (const class_name &)  = delete; \
    protected: class_name (const class_name &&) = delete;

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

    template<typename class_t>
    class virtual_copyable
    {
    public:
        virtual class_t* copy() const = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    class i_as_string
    {
    public:
        virtual std::string asString() const = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename T>
    inline string make_string (T arg)
        { return to_string (arg);  }

    template<>
    inline string make_string<string> (string arg)
        { return arg; }

    template<>
    inline string make_string<const char*> (const char *arg)
        { return string (arg); }


    template<typename T1>
    inline string mkstr (T1 x1)
        { return make_string (x1); }


    template<typename T1, typename T2>
    inline string mkstr (T1 x1, T2 x2)
        { return mkstr (x1).append (make_string (x2)); }


    template<typename T1, typename T2, typename T3>
    inline string mkstr (T1 x1, T2 x2, T3 x3)
        { return mkstr (x1, x2).append (make_string (x3)); }


    template<typename T1, typename T2, typename T3, typename T4>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4)
        { return mkstr (x1, x2, x3).append (make_string (x4)); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5)
        { return mkstr (x1, x2, x3, x4).append (make_string (x5)); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6)
        { return mkstr (x1, x2, x3, x4, x5).append (make_string (x6)); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7)
        { return mkstr (x1, x2, x3, x4, x5, x6).append (make_string (x7)); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8)
        { return mkstr (x1, x2, x3, x4, x5, x6, x7).append (make_string (x8)); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9)
        { return mkstr (x1, x2, x3, x4, x5, x6, x7, x8).append (make_string (x9)); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
    inline string mkstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10)
        { return mkstr (x1, x2, x3, x4, x5, x6, x7, x8, x9).append (make_string (x10)); }

//----------------------------------------------------------------------------------------------------------------------

    template<typename T1>
    inline string mkfstr (T1 x1)
        { return ((std::ostringstream &)(std::ostringstream() << x1)).str(); }


    template<typename T1, typename T2>
    inline string mkfstr (T1 x1, T2 x2)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2)).str(); }


    template<typename T1, typename T2, typename T3>
    inline string mkfstr (T1 x1, T2 x2, T3 x3)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3)).str(); }


    template<typename T1, typename T2, typename T3, typename T4>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4)).str(); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4 << x5)).str(); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4 << x5 << x6)).str(); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4 << x5 << x6 << x7)).str(); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8)).str(); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9)).str(); }


    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
    inline string mkfstr (T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10)
        { return ((std::ostringstream &)(std::ostringstream() << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10)).str(); }


//----------------------------------------------------------------------------------------------------------------------

    template<typename... handler_args_t>
    class event :
        public non_copyable
    {
    public:
        typedef  std::function<void(handler_args_t...)>       handler_t;
        typedef  typename vector<handler_t>::const_iterator   handler_id;

    protected:
        vector<handler_t> _handlers;

    public:
        event()
        {
            _handlers.reserve (2);
        }


        inline void fire (const handler_args_t&... arguments)
        {
            for (auto nextHandler : _handlers)  nextHandler (arguments...);
        }


        inline void operator() (const handler_args_t&... arguments)
        {
            fire (arguments...);
        }


        inline handler_id handleWith (handler_t handler)
        {
            _handlers.emplace_back (handler);

            auto lastIt = _handlers.end();
            std::advance (lastIt, -1);
            return lastIt;
        }


        inline void stopHandlingWith (handler_id handler)
        {
            _handlers.erase (handler);
        }


        void disconnectAllHandlers()
        {
            _handlers.clear();
        }
    };


#define event_access(public_name, private_name)                       \
    inline decltype(private_name)& public_name##Event()               \
    {                                                                 \
        return private_name;                                          \
    }

//----------------------------------------------------------------------------------------------------------------------

    #define declare_ptr(object_t)          typedef shared_ptr<object_t> ptr;

    #define declare_ptr_alloc(object_t)    declare_ptr (object_t)                                    \
                                           template<typename... args_t>                              \
                                           inline static typename object_t::ptr alloc (args_t&&... ctorArgs) \
                                           { return make_shared<object_t> (std::forward<args_t> (ctorArgs)...); }           
}

//----------------------------------------------------------------------------------------------------------------------

#endif
