#include "world.hh"

void World::update(double delta)
{
	
}

std::vector<SP<Object>> World::getObjectsByName(const std::string & name)
{
	std::vector<SP<Object>> out{};
	for(auto &obj : this->objects)
	{
		if(obj->name == name) out.push_back(obj);
	}
	return out;
}

std::vector<SP<Object>> World::getObjectsOnLayer(uint64_t layer)
{
	std::vector<SP<Object>> out{};
	for(auto const &obj : this->objects)
	{
		if(obj->graphicsComp && obj->graphicsComp->layer == layer) out.push_back(obj);
	}
	return out;
}

std::vector<SP<Object>> World::getObjectsAtPosition(const vec2<float> & pos)
{
	std::vector<SP<Object>> out{};
	for(auto const &obj : this->objects)
	{
		if(obj->spatialComp && obj->spatialComp->pos.x() == pos.x() && obj->spatialComp->pos.y() == pos.y()) out.push_back(obj);
	}
	return out;
}

struct RenderList World::getSceneGraph()
{
	RenderList out;
	for(auto const &obj : this->objects)
	{
		if(obj->graphicsComp && obj->spatialComp)
		{
			Renderable r{
				obj->spatialComp->pos,
				obj->spatialComp->scale,
				obj->spatialComp->rotation,
				vec3<double>{0.0, 0.0, 1.0},
				obj->graphicsComp->atlasID,
				obj->graphicsComp->shaderID,
				obj->graphicsComp->layer,
				obj->graphicsComp->subLayer,
				obj->name};
			out.add(r);
		};
	}
	return out;
}
