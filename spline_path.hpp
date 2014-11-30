
#ifndef __spline_path_included__
#define __spline_path_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "camera.hpp"
#include "render_window.hpp"

using namespace math3d;

//----------------------------------------------------------------------------------------------------------------------

class spline_path :
	public oo_extensions::non_copyable
{
public:
	struct spline_node
	{
		vector3_d  position;
		vector2_d  angles;
		double     duration;
	};


private:
	vector<spline_node>  _path;
	double _wholeDuration = 0;

	unsigned _currentNodeIndex = 0;
	double _timeSinceLastNode = 0;
	double _lastUpdateTime = -1;
	render::camera::ptr  _camera = nullptr;

	event<const render_window&>::handler_id  _updateHandler;
	event<const render_window&>  *_updateEvent = nullptr;

public:
	property_get (Duration, _wholeDuration)
	property_get (ControlledCamera, _camera)


private:
	inline  double _hermite00 (double t)    { return  2*t*t*t - 3*t*t + 1; }
	inline  double _hermite10 (double t)    { return    t*t*t - 2*t*t + t; }
	inline  double _hermite01 (double t)    { return -2*t*t*t + 3*t*t;     }
	inline  double _hermite11 (double t)    { return    t*t*t -   t*t;     }

	inline  spline_node&  _currentNode()    { return _path[_currentNodeIndex]; }

	double    _catmullRom (double yn1, double y0, double y1, double y2, double dx, double t);
	vector3_d _catmullRom (const vector3_d &yn1, const vector3_d &y0, const vector3_d &y1,
						   const vector3_d &y2, double dx, double t);
	vector2_d _catmullRom (const vector2_d &yn1, const vector2_d &y0, const vector2_d &y1,
			const vector2_d &y2, double dx, double t);

	void _onUpdatePlay();


public:
	declare_ptr_alloc (spline_path)

	spline_path (vector<spline_node> &&nodes);
	spline_path (const string& fileName);

	void save (const string& fileName);
	
	void playOnCamera (render::camera::ptr cam, event<const render_window&> &updateEvent);
	void stopPlaying();
	void resetPlaying();

	bool isPlayingFinished();
	bool isPlayingStarted();

	void clear();
};

//----------------------------------------------------------------------------------------------------------------------

class spline_path_recorder
{
private:
	vector<spline_path::spline_node>  _path;
	render::camera::ptr  _camera;

	event<const render_window&>::handler_id  _updateHandler;
	event<const render_window&>  *_updateEvent = nullptr;

	double _timeSinceLastNode = 0;
	double _lastUpdateTime = -1;


private:
	void _onUpdatePlay();

public:
	declare_ptr_alloc (spline_path_recorder)

	void recordFromCamera (render::camera::ptr cam, event<const render_window&> &updateEvent);
	spline_path::ptr stopRecording();
};

//----------------------------------------------------------------------------------------------------------------------

#endif
