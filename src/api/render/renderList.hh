#pragma once

#include <functional>
#include <cstdint>
#include <commons/math/vec2.hh>
#include <commons/math/vec3.hh>
#include <vector>

struct Renderable
{
	Renderable(vec2<double> const &pos, vec2<double> const &scale, double rotation, vec3<double> const &axis, uint64_t atlasID, uint64_t shaderID, size_t layer, size_t sublayer, std::string const &name) :
			pos(pos), scale(scale), rotation(rotation), axis(axis), atlasID(atlasID), shaderID(shaderID), layer(layer), sublayer(sublayer), name(name) {}
	vec2<double> pos{}, scale{};
	double rotation = 0.0f;
	vec3<double> axis{0.0f, 0.0f, 1.0f};
	uint64_t atlasID = 0, shaderID = 0;
	size_t layer = 0, sublayer = 0;
	std::string name;
};

struct RenderList
{
	using Comparator = std::function<bool(Renderable const &a, Renderable const &b)>;
	
	[[nodiscard]] inline static bool renderableComparator(Renderable const &a, Renderable const &b)
	{
		return (a.atlasID > b.atlasID) && (a.layer == b.layer) ? a.sublayer > b.sublayer : a.layer > b.layer;
	}
	
	[[nodiscard]] inline Renderable& operator [](size_t index)
	{
		return this->list[index];
	}
	
	inline void add(std::initializer_list<Renderable> const &renderables)
	{
		this->list.insert(this->list.end(), renderables.begin(), renderables.end());
	}
	
	inline void clear()
	{
		this->list.clear();
	}
	
	[[nodiscard]] inline bool empty() const
	{
		return this->list.empty();
	}
	
	[[nodiscard]] inline size_t size() const
	{
		return this->list.size();
	}
	
	inline void sort(Comparator const &cmp = RenderList::renderableComparator)
	{
		std::sort(this->list.begin(), this->list.end(), cmp);
	}
	
	std::vector<Renderable> list;
};
