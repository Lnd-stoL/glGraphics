
#include "render_resources.hpp"

#include "resource_manager_impl.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    void resources::addFilesSearchLocation (const string &path)
    {
        _gpuProgramsManager.addFileSearchLocation (path);
        _meshesManager.addFileSearchLocation (path);
        _fontsManager.addFileSearchLocation (path);
        _vertexShadersManager.addFileSearchLocation (path);
        _fragmentShadersManager.addFileSearchLocation (path);
        _geometryShadersManager.addFileSearchLocation (path);
        _texturesManager.addFileSearchLocation (path);
    }


    void resources::releaseUnused()
    {
        _gpuProgramsManager.releaseUnused();
        _meshesManager.releaseUnused();
        _fontsManager.releaseUnused();
        _vertexShadersManager.releaseUnused();
        _fragmentShadersManager.releaseUnused();
        _geometryShadersManager.releaseUnused();
        _texturesManager.releaseUnused();
    }
}
