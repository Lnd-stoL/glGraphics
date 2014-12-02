
#include "config_file.hpp"
#include "debug.hpp"

//----------------------------------------------------------------------------------------------------------------------

config_set::config_set (const string &fileName)
{
    debug::log::println (mkstr ("loading configuration file '", fileName, "'"));
    try { ps::info_parser::read_info (fileName, _prop); }

    catch (const ps::info_parser_error &parserError)
    {
        debug::log::println_err (mkstr ("error parsing .info format: ", parserError.message()));
        return;
    }

    catch (const ps::file_parser_error &generalParserError)
    {
        debug::log::println_err (mkstr ("error parsing file: ", generalParserError.message()));
        return;
    }

    resource::_loaded();
}


config_set::config_set (const ps::ptree &ptr)
    : _prop (ptr)
{
}


config_set::ptr  config_set::getGroup (string const &name)  const
{
    return make_shared<config_set> (_prop.get_child (_pathFromString (name)));
}


config_set::path_t  config_set::_pathFromString (string const &strPath) const
{
    return path_t (strPath);
}
