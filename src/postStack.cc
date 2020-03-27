#include "postStack.hh"

void LayerPostStack::addToLayer(uint64_t layer, SP<RenderPass> const &renderPass)
{
	if(this->postOrder.find(layer) != this->postOrder.end()) this->postOrder[layer] = std::vector<SP<RenderPass>>{};
	this->postOrder[layer].push_back(renderPass);
}

void LayerPostStack::removeFromLayer(uint64_t layer, SP<RenderPass> const &renderPass)
{
	if(!this->contains(layer)) return;
	this->postOrder[layer].erase(std::find(this->postOrder[layer].begin(), this->postOrder[layer].end(), renderPass));
}

bool LayerPostStack::contains(uint64_t layer)
{
	return this->postOrder.find(layer) != this->postOrder.end();
}

std::vector<SP<RenderPass>> LayerPostStack::getPassesForLayer(uint64_t layer)
{
	if(this->contains(layer)) return this->postOrder[layer];
	else return {};
}

bool LayerPostStack::empty()
{
	return this->postOrder.empty();
}

void GlobalPostStack::add(SP<RenderPass> const &renderPass)
{
	this->postOrder.push_back(renderPass);
}

void GlobalPostStack::remove(SP<RenderPass> const &renderPass)
{
	this->postOrder.erase(std::find(this->postOrder.begin(), this->postOrder.end(), renderPass));
}

std::vector<SP<RenderPass>> GlobalPostStack::getPasses()
{
	return this->postOrder;
}

bool GlobalPostStack::empty()
{
	return this->postOrder.empty();
}
