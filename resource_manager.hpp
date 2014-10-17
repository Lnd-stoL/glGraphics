
#ifndef __resource_manager_included__
#define __resource_manager_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "resource.hpp"
#include "gpu_program.hpp"

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

public:
    void addFileSearchLocation (const string &location);
    typename resource_t::ptr acquireFromFile (const string &fileName, const vector<string> &additionalSearchLocations = vector<string>());

    typename resource_t::ptr request (const resource::identifyer &resourceId);
};

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class resources
    {
        resource_manager<gpu_program> _gpuProgramsManager;

    public:
        property_ref (GpuProgramsManager, _gpuProgramsManager)
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif