#pragma once

#include "object.hh"
#include "api/render/renderList.hh"

#include <vector>

struct World
{
	/// Advance the world's simulation 1 tick
	/// Main thread calls this function before each frame is rendered
	/// \param delta Time since the last frame occured, used to make movement FPS independant (objects wont jump around if game is lagging)
	void update(double delta);
	
	/// Get all objects with the given name in this world
	/// \param name The name of the object(s) you're looking for
	std::vector<SP<Object>> getObjectsByName(std::string const &name);
	
	/// Get all objects on a given layer in this world
	/// \param layer The layer the object(s) you're looking for are on
	std::vector<SP<Object>> getObjectsOnLayer(uint64_t layer);
	
	/// Get all objects at a given position in this world
	/// \param pos The position of the object(s) you're looking for
	std::vector<SP<Object>> getObjectsAtPosition(vec2<float> const &pos);
	
	/// Get a representation of the visible objects in this world that can be given to the renderer to render this world
	RenderList getSceneGraph();
	
	std::vector<SP<Object>> objects;
};
