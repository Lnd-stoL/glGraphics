
#include "oo_extensions.hpp"
#include "resource.hpp"
#include "debug.hpp"

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using oo_extensions::mkstr;
namespace ps = boost::property_tree;

//----------------------------------------------------------------------------------------------------------------------

class config_set : public resource
{
    ps::ptree  _prop;

public:
    property_ref (propTree, _prop)


protected:
    typedef  ps::string_path<string, ps::id_translator<string>>  path_t;
    path_t  _pathFromString (const string& strPath)  const;

public:
    declare_ptr_alloc (config_set)

    explicit config_set (const string &fileName);
    config_set (const ps::ptree &ptr);

    config_set::ptr  getGroup (const string &name)  const;


    template<typename value_t>
    inline value_t get (const string &name)  const
    {
        try { return (value_t) _prop.get<value_t> (_pathFromString (name)); }
        catch (ps::ptree_error ptreeError)
        {
            debug::log::println_err (mkstr ("property tree error getting '", name, "': ", ptreeError.what ()));
        }

        return value_t ();
    }
};
