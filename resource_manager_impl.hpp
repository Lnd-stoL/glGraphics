
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
typename resource_t::ptr resource_manager<resource_t>::
acquireFromFile (const string &fileName, const vector<string> &additionalSearchLocations)
{
    debug::log::println (mkstr ("resource manager: acquired '", fileName, "'"));

    fs::path foundIn;
    if (!_findFile (fileName, foundIn, additionalSearchLocations))  return typename resource_t::ptr();
    foundIn = fs::absolute (foundIn);

    auto foundInAlreadyLoaded = _loadedResources.find (foundIn.string());
    if (foundInAlreadyLoaded != _loadedResources.end())  return foundInAlreadyLoaded->second;

    auto loadedResource = resource_t::alloc (foundIn.string());
    _loadedResources[foundIn.string()] = loadedResource;
    return loadedResource;
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
typename resource_t::ptr resource_manager<resource_t>::request (const resource::identifyer &resourceId)
{
    string hashString = resourceId.hashString();
    debug::log::println (mkstr ("resource manager: acquired resource with hash '", hashString, "'"));

    auto foundInAlreadyLoaded = _loadedResources.find (hashString);
    if (foundInAlreadyLoaded != _loadedResources.end())  return foundInAlreadyLoaded->second;

    auto loadedResource = resource_t::alloc (resourceId);
    _loadedResources[hashString] = loadedResource;
    return loadedResource;
}
