
#include "resource_manager.hpp"

#include <algorithm>
#include <boost/range/join.hpp>

//----------------------------------------------------------------------------------------------------------------------

template<typename resource_t>
void resource_manager<resource_t>::addFileSearchLocation (const string &location)
{
    _fileLocations.insert (location);
}


template<typename resource_t>
string resource_manager<resource_t>::locateFile (string filename)
{
    fs::path locatedFile;
    _findFile (filename, locatedFile, vector<string>());
    return locatedFile.string();
}


template<typename resource_t>
bool resource_manager<resource_t>::
_findFile (const string &fileName, fs::path& foundIn, const vector<string> &additionalSearchLocations)
{
    fs::path filePath (fileName);
    if (!filePath.is_absolute())
    {
        for (auto nextFindLoc : boost::join (_fileLocations, additionalSearchLocations))
        {
            fs::path supposedPath = fs::path (nextFindLoc) / fileName;
            supposedPath.normalize();
            if (fs::exists (supposedPath))
            {
                foundIn = supposedPath;
                debug::log::println (mkstr ("resource manager: found file '", foundIn.string(), "'"));
                return true;
            }
        }
    }

    if (!fs::exists (filePath))
    {
        debug::log::println_err (mkstr ("resource manager can't find file '", fileName, "'"));
        return false;
    }

    foundIn = filePath;
    return true;
}


template<typename resource_t>
template<typename ...args_t>
typename resource_t::ptr resource_manager<resource_t>::_request (const string &hashString, args_t&&... ctorArgs)
{
    debug::log::println (mkstr ("resource manager: requested resource with hash '", hashString, "'"));

    auto foundInAlreadyLoaded = _loadedResources.find (hashString);
    if (foundInAlreadyLoaded != _loadedResources.end() && foundInAlreadyLoaded->second->isValidAsResource())
        return foundInAlreadyLoaded->second;

    auto loadedResource = resource_t::alloc (std::forward<args_t> (ctorArgs)...);
    _loadedResources[hashString] = loadedResource;
    return loadedResource;
}


template<typename resource_t>
template<typename ...args_t>
typename resource_t::ptr resource_manager<resource_t>::
_requestFromFile (const string &fileName, const vector<string> &additionalSearchLocations, args_t&&... ctorArgs)
{
    fs::path foundIn;
    if (!_findFile (fileName, foundIn, additionalSearchLocations))  return typename resource_t::ptr();
    foundIn = fs::absolute (foundIn);

    return _request (foundIn.string(), foundIn.string(), std::forward<args_t> (ctorArgs)...);
}


template<typename resource_t>
template<typename ...args_t>
typename resource_t::ptr resource_manager<resource_t>::request (const typename resource_t::id &resourceId, args_t&&... ctorArgs)
{
    return _request (resourceId.hashString(), resourceId, std::forward<args_t> (ctorArgs)...);
}


template<typename resource_t>
template<typename ...args_t>
typename resource_t::ptr resource_manager<resource_t>::
request (const string &fileName, const vector<string> &additionalSearchLocations, args_t&&... ctorArgs)
{
    return _requestFromFile (fileName, additionalSearchLocations, std::forward<args_t> (ctorArgs)...);
}


template<typename resource_t>
template<typename ...args_t>
typename resource_t::ptr resource_manager<resource_t>::request (const string &fileName, args_t&&... ctorArgs)
{
    return _requestFromFile (fileName, vector<string>(), std::forward<args_t> (ctorArgs)...);
}
