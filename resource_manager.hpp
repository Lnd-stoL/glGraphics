
#ifndef __resource_manager_included__
#define __resource_manager_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"

#include <unordered_map>
#include <unordered_set>
#include <boost/filesystem.hpp>

using oo_extensions::mkstr;
namespace fs = boost::filesystem;

//----------------------------------------------------------------------------------------------------------------------

template<typename resource_t>
class resource_manager:
    public oo_extensions::non_copyable
{
    std::unordered_set<string> _fileLocations;
    std::unordered_map<string, typename resource_t::ptr> _loadedResources;

protected:
    bool _findFile (const string &fileName, fs::path& foundIn, const vector<string> &additionalSearchLocations);

    template<typename ...args_t>
    typename resource_t::ptr _request (const string& hashString, args_t&&... ctorArgs);

    template<typename ...args_t>
    typename resource_t::ptr _requestFromFile (const string& fileName, const vector<string> &additionalSearchLocations, args_t&&... ctorArgs);


public:
    void addFileSearchLocation (const string &location);
    string locateFile (string filename);

    template<typename ...args_t>
    typename resource_t::ptr request (const typename resource_t::id &resourceId, args_t&&... ctorArgs);

    template<typename ...args_t>
    typename resource_t::ptr requestWithAdditionalLoactions (const string &fileName,
                                                             const vector<string> &additionalSearchLocations,
                                                             args_t&&... ctorArgs);

    template<typename ...args_t>
    typename resource_t::ptr request (const string &fileName, args_t&&... ctorArgs);

    void releaseUnused();
};

//----------------------------------------------------------------------------------------------------------------------

#endif
