
#ifndef __scene__included__
#define __scene__included__

//----------------------------------------------------------------------------------------------------------------------

#include "renderable.hpp"
#include "material.hpp"
#include <set>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class graphics_renderer;

//----------------------------------------------------------------------------------------------------------------------

    class scene_object
    {
        int             _renderQueue;
        renderable::ptr _renderableObject;

    public:
        property_get (renderQueue, _renderQueue)
        property_get (underlyingRenderable,  _renderableObject)

    public:
        declare_ptr_alloc (scene_object)

        scene_object (renderable::ptr renderableObject, int renderQueue = 0) : _renderQueue (renderQueue),
                                                                               _renderableObject (renderableObject)
        { }


        bool operator< (const scene_object& theOtherSceneObject)
        {
            if (_renderQueue != theOtherSceneObject._renderQueue)
                return _renderQueue < theOtherSceneObject._renderQueue;

            return ((const renderable*) _renderableObject.get()) < ((const renderable*) theOtherSceneObject._renderableObject.get());
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    class scene : public renderable
    {
    public:
        typedef unsigned  render_group_id;
        static const render_group_id  defaultRenderGroupId = (render_group_id) -1;


    private:
        typedef std::function<bool (const scene_object::ptr&, const scene_object::ptr&)>  sceneObjectComparer_t;
        typedef std::set<scene_object::ptr, sceneObjectComparer_t>  objects_set_t;

        vector<objects_set_t> _renderGroups;

        std::map<string, render_group_id> _renderGroupsNames;
        render_group_id  _defaultRenderGroup = 0;

        math3d::vector3_d  _sunPosition;
        color_rgb<float>  _sunColor;


    public:
        property_get_ref (sunPosition,  _sunPosition)
        property_get_ref (sunColor,  _sunColor);

        property_get (renderingGroupsCount,  _renderGroups.size());


    protected:
        inline objects_set_t &_defaultObjectsSet()  { return _renderGroups[_defaultRenderGroup]; }

    public:
        declare_ptr_alloc (scene)
        scene();

        render_group_id addRenderGroup (const string &name);
        render_group_id defaultRenderGroup()  const;

        void selectRenderGroup (const string &name);
        void defaultRenderGroup (render_group_id groupId);

        string renderGroup (render_group_id groupId)  const;
        render_group_id renderGroup (const string &name)  const;

        scene_object::ptr addRenderableObject (renderable::ptr renderableObject, int renderQueue = 0);
        scene_object::ptr addRenderableObject (const string &renderGroupName, renderable::ptr renderableObject, int renderQueue = 0);
        scene_object::ptr addRenderableObject (render_group_id renderGroupId, renderable::ptr renderableObject, int renderQueue = 0);

        void addSceneObject (render_group_id renderGroupId, scene_object::ptr sceneObject);
        void removeSceneObject (scene_object::ptr sceneObject);
        void removeSceneObject (render_group_id renderGroupId, scene_object::ptr sceneObject);

        void setSun (math3d::vector3_d position, color_rgb<float> color);

        void draw (graphics_renderer &renderer) const;
        void draw (graphics_renderer &renderer, render_group_id renderGroupId) const;
        void draw (graphics_renderer &renderer, const string &renderGroupName) const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
