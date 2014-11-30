
#include "spline_path.hpp"

#include "GLFW/glfw3.h"

//----------------------------------------------------------------------------------------------------------------------

spline_path::spline_path (vector<spline_path::spline_node> &&nodes)
{
	if (nodes.size() < 2)
	{
		debug::log::println_err ("can't constuct spline path from nodes; at least 2 nodes required");
		return;
	}

	_path.swap (nodes);
}


spline_path::spline_path (const string &fileName)
{
	debug::log::println (mkstr ("loading path from '", fileName, "'"));

	std::ifstream file (fileName);
	if (!file.good())
	{
		debug::log::println_err (mkstr ("can't load path from '", fileName, "' since it can't be found"));
		return;
	}

	int nodeCount = -1;
	string nextLine;
	while (!file.eof())
	{
		std::getline (file, nextLine);
		if (nextLine.empty())  continue;
		std::istringstream parser (nextLine);

		if (nodeCount < 0)
		{
			parser >> nodeCount;

			if (nodeCount < 4)
			{
				debug::log::println_err ("path is too short; it has to be at least 4 points length");
				file.close();
				return;
			}

			_path.reserve ((size_t) nodeCount);
		}

		else
		{
			double px = 0, py = 0, pz = 0, rx = 0, ry = 0, rz = 0, rw = 0, t = 0;
			parser >> px >> py >> pz >> rx >> ry >> rz >> rw >> t;

			spline_node nextNode;
			nextNode.position = vector3_d (px, py, pz);
			nextNode.rotation = quaternion_d (rx, ry, rz, rw);
			nextNode.duration = t;
			_wholeDuration += t;

			_path.emplace_back (std::move (nextNode));
		}
	}

	file.close();
	debug::log::println_mrk (mkstr ("path successfully loaded; ", _path.size(), " points; ", _wholeDuration, " sec"));
}


void spline_path::save (const string &fileName)
{
	debug::log::println (mkstr ("saving path to '", fileName, "'"));

	std::ofstream file (fileName);
	if (!file.good())
	{
		debug::log::println_err ("failed to create an output file");
		return;
	}

	file << _path.size() << std::endl;
	for (auto& nextNode : _path)
	{
		file << nextNode.position.getX() << " " << nextNode.position.getY() << " " << nextNode.position.getZ() << " ";
		file << nextNode.rotation.getX() << " " << nextNode.rotation.getY() << " " << nextNode.rotation.getZ() << " " <<
				nextNode.rotation.getW() << " ";
	    file << nextNode.duration << "\n";
	}

	file.close();
	debug::log::println_mrk (mkstr ("path successfully saved to file"));
}


double spline_path::_catmullRom (double yn1, double y0, double y1, double y2, double dx, double t)
{
	double m0 = (y1 - yn1) / 2;
	double m1 = (y2 - y0)  / 2;

	return _hermite00 (t) * y0 + _hermite10 (t) * m0 * dx +
		   _hermite01 (t) * y1 + _hermite11 (t) * m1 * dx;
}


vector3_d spline_path::_catmullRom (const vector3_d &yn1, const vector3_d &y0, const vector3_d &y1, const vector3_d &y2,
								    double dx, double t)
{
	return vector3_d (
			_catmullRom (yn1.getX(), y0.getX(), y1.getX(), y2.getX(), dx, t),
			_catmullRom (yn1.getY(), y0.getY(), y1.getY(), y2.getY(), dx, t),
			_catmullRom (yn1.getZ(), y0.getZ(), y1.getZ(), y2.getZ(), dx, t)
	);
}


vector2_d spline_path::_catmullRom (const vector2_d &yn1, const vector2_d &y0, const vector2_d &y1, const vector2_d &y2,
								    double dx, double t)
{
	return vector2_d (
			_catmullRom (yn1.getX(), y0.getX(), y1.getX(), y2.getX(), dx, t),
			_catmullRom (yn1.getY(), y0.getY(), y1.getY(), y2.getY(), dx, t)
	);
}


quaternion_d spline_path::_catmullRom (const quaternion_d &yn1, const quaternion_d &y0, const quaternion_d &y1,
									   const quaternion_d &y2, double dx, double t)
{
	return quaternion_d (
			_catmullRom (yn1.getX(), y0.getX(), y1.getX(), y2.getX(), dx, t),
			_catmullRom (yn1.getY(), y0.getY(), y1.getY(), y2.getY(), dx, t),
			_catmullRom (yn1.getZ(), y0.getZ(), y1.getZ(), y2.getZ(), dx, t),
			_catmullRom (yn1.getW(), y0.getW(), y1.getW(), y2.getW(), dx, t)
	);
}


void spline_path::playOnCamera (render::camera::ptr cam, event<const render_window&> &updateEvent)
{
	stopPlaying();
	resetPlaying();

	_camera = cam;
	_updateEvent = &updateEvent;

	_updateHandler = updateEvent.handleWith ([this] (const render_window &) {
		this->_onUpdatePlay();
	});
}


void spline_path::stopPlaying()
{
	if (_updateEvent)  _updateEvent->stopHandlingWith (_updateHandler);

	_updateEvent = nullptr;
	_updateHandler = event<const render_window&>::handler_id();
}


void spline_path::resetPlaying()
{
	_camera = nullptr;
	_currentNodeIndex = 0;
	_lastUpdateTime = -1;
	_timeSinceLastNode = 0;
}


void spline_path::clear()
{
	_path.clear();
}


bool spline_path::isPlayingFinished()
{
	return _currentNodeIndex > _path.size() - 2;
}


bool spline_path::isPlayingStarted()
{
	return _lastUpdateTime >= 0;
}


void spline_path::_onUpdatePlay()
{
	double curTime = glfwGetTime();
	if (_lastUpdateTime < 0)  _lastUpdateTime = curTime;
	_timeSinceLastNode += curTime - _lastUpdateTime;
	_lastUpdateTime = curTime;

	if (_timeSinceLastNode >= _currentNode().duration * _playSpeed)
	{
		++_currentNodeIndex;
		if (isPlayingFinished())
		{
			stopPlaying();
			return;
		}

		_timeSinceLastNode = 0;
	}

	const double dx = 1.0;
	double t = _timeSinceLastNode / _currentNode().duration;

	int yn1Index = _currentNodeIndex - 1;
	int y2Index =  _currentNodeIndex + 2;
	if (yn1Index < 0)  yn1Index = 0;
	if (y2Index == _path.size())  y2Index = (int) _path.size() - 1;

	const spline_node &yn1 = _path[yn1Index];
	const spline_node &y0  = _path[_currentNodeIndex + 0];
	const spline_node &y1  = _path[_currentNodeIndex + 1];
	const spline_node &y2  = _path[y2Index];

	vector3_d nextPosition    = _catmullRom (yn1.position, y0.position, y1.position, y2.position, dx, t);
	quaternion_d nextRotation = _catmullRom (yn1.rotation, y0.rotation, y1.rotation, y2.rotation, dx, t);

	_camera->changeTransform (nextPosition, rotation_d (nextRotation));
}

//----------------------------------------------------------------------------------------------------------------------

void spline_path_recorder::recordFromCamera (render::camera::ptr cam,
											 event<const render_window&> &updateEvent,
								             double samplingRate)
{
	if (_camera)
	{
		debug::log::println_err ("can't start recording a path while another path is being recorded");
		return;
	}

	_samplingRate = samplingRate;
	_camera = cam;
	_updateEvent = &updateEvent;

	_lastUpdateTime = -1;
	_timeSinceLastNode = 0;

	_updateHandler = updateEvent.handleWith ([this] (const render_window &) {
		this->_onUpdatePlay();
	});
}


spline_path::ptr spline_path_recorder::stopRecording()
{
	if (_updateEvent)  _updateEvent->stopHandlingWith (_updateHandler);

	_updateEvent = nullptr;
	_updateHandler = event<const render_window&>::handler_id();

	if (_path.size() == 0)
	{
		debug::log::println_err ("can't stop recording properly; the path cotains no points");
		return spline_path::ptr();
	}

	if (_path.size() == 1)
	{
		_path.push_back (_path[0]);
	}

	auto spline =  spline_path::alloc (std::move (_path));
	_path.clear();
	_lastUpdateTime = -1;
	_timeSinceLastNode = 0;
	_camera = nullptr;

	return spline;
}


void spline_path_recorder::_onUpdatePlay()
{
	double curTime = glfwGetTime();
	if (_lastUpdateTime < 0)  _lastUpdateTime = curTime;
	_timeSinceLastNode += curTime - _lastUpdateTime;
	_lastUpdateTime = curTime;

	if (_timeSinceLastNode >= _samplingRate || _lastUpdateTime < 0)
	{
		const transform_d &camTransform = _camera->getTransform();

		spline_path::spline_node nextNode;
		nextNode.position = camTransform.getTranslation();
		nextNode.rotation = camTransform.getRotation().asQuaternion();
		nextNode.duration = _timeSinceLastNode;

		_path.emplace_back (std::move (nextNode));
		_timeSinceLastNode = 0;
	}
}
