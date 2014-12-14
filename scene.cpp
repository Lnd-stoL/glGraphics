
#include "scene.hpp"
#include "graphics_renderer.hpp"
#include <algorithm>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    scene::scene()
    {
        addRenderGroup ("default");
        defaultRenderGroup (0);
    }


    void scene::removeSceneObject (scene_object::ptr sceneObject)
    {
        for (auto &nextGroup : _renderGroups)
        {
            nextGroup.erase (sceneObject);
        }
    }


    void scene::removeSceneObject (render_group_id renderGroupId, scene_object::ptr sceneObject)
    {
        _renderGroups[renderGroupId].erase (sceneObject);
    }


    scene_object::ptr  scene::addRenderableObject (renderable::ptr renderableObject, int renderQueue)
    {
        return addRenderableObject (_defaultRenderGroup, renderableObject, renderQueue);
    }


    scene_object::ptr  scene::addRenderableObject (const string &renderGroupName, renderable::ptr renderableObject,
                                                   int renderQueue)
    {
        return addRenderableObject (renderGroup (renderGroupName), renderableObject, renderQueue);
    }


    scene_object::ptr  scene::addRenderableObject (scene::render_group_id renderGroupId, renderable::ptr renderableObject,
                                                   int renderQueue)
    {
        auto sceneObject = scene_object::alloc (renderableObject, renderQueue);     // TODO: Ineffective

        addSceneObject (renderGroupId, sceneObject);
        return sceneObject;
    }


    void scene::addSceneObject (render_group_id renderGroupId, scene_object::ptr sceneObject)
    {
        _renderGroups[renderGroupId].insert (sceneObject);
    }


    void scene::draw (graphics_renderer &renderer) const
    {
        draw (renderer, _defaultRenderGroup);
    }


    void scene::setDirectionalLight (math3d::vector3_f position, color_rgb<float> color)
    {
        _sunPosition = position;
        _sunColor = color;
    }


    scene::render_group_id  scene::addRenderGroup (const string &name)
    {
        if (_renderGroupsNames.find (name) != _renderGroupsNames.end())
        {
            debug::log::println_err (mkstr ("failed to add render group '", name, "'; exists already"));
            return (scene::render_group_id) -1;
        }

        auto setComparer = [] (const scene_object::ptr& obj1, const scene_object::ptr& obj2)
        {
            return *obj1 < *obj2;
        };

        _renderGroups.emplace_back (objects_set_t (setComparer));
        auto newId = (scene::render_group_id) _renderGroups.size() - 1;
        _renderGroupsNames[name] = newId;

        return newId;
    }


    void scene::selectRenderGroup (const string &name)
    {
        _defaultRenderGroup = renderGroup (name);
    }


    void scene::defaultRenderGroup (scene::render_group_id groupId)
    {
        if (groupId == defaultRenderGroupId)  groupId = this->defaultRenderGroup();

        if (unsigned (groupId) > _renderGroups.size())
        {
            debug::log::println_err (mkstr ("failed to set default render group ", unsigned (groupId)));
            return;
        }

        _defaultRenderGroup = groupId;
    }


    scene::render_group_id  scene::defaultRenderGroup()  const
    {
        return _defaultRenderGroup;
    }


    string  scene::renderGroup (scene::render_group_id groupId)  const
    {
        if (groupId == defaultRenderGroupId)  groupId = this->defaultRenderGroup();

        for (auto next : _renderGroupsNames)
        {
            if (next.second == groupId)  return next.first;
        }

        return "";
    }


    scene::render_group_id  scene::renderGroup (const string &name)  const
    {
        auto renderGroupId = _renderGroupsNames.find (name);
        if (renderGroupId == _renderGroupsNames.end())
        {
            debug::log::println_err (mkstr ("failed locate render group '", name, "'"));
            return (scene::render_group_id) -1;
        }

        return renderGroupId->second;
    }


    void scene::draw (graphics_renderer &renderer, scene::render_group_id renderGroupId) const
    {
        if (renderGroupId == defaultRenderGroupId)  renderGroupId = this->defaultRenderGroup();

        auto &group = _renderGroups[unsigned (renderGroupId)];
        for (auto &nextObj : group)
        {
            nextObj->underlyingRenderable()->draw (renderer);
        }
    }


    void scene::draw (graphics_renderer &renderer, const string &renderGroupName) const
    {
        draw (renderer, renderGroup (renderGroupName));
    }
}
