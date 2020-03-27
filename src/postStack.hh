#pragma once

#include "def.hh"
#include "api/render/renderPass.hh"

#include <algorithm>
#include <unordered_map>

struct LayerPostStack
{
	void addToLayer(uint64_t layer, SP<RenderPass> const &renderPass);
	void removeFromLayer(uint64_t layer, SP<RenderPass> const &renderPass);
	[[nodiscard]] std::vector<SP<RenderPass>> getPassesForLayer(uint64_t layer);
	[[nodiscard]] bool empty();

private:
	[[nodiscard]] bool contains(uint64_t layer);
	
	std::unordered_map<uint64_t, std::vector<SP<RenderPass>>> postOrder;
};

struct GlobalPostStack
{
	void add(SP<RenderPass> const &renderPass);
	void remove(SP<RenderPass> const &renderPass);
	[[nodiscard]] std::vector<SP<RenderPass>> getPasses();
	[[nodiscard]] bool empty();

private:
	std::vector<SP<RenderPass>> postOrder;
};
