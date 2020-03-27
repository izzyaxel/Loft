#include "components.hh"
#include "assets.hh"
#include "util.hh"

void PlayerComponent::bind(Key key, Action action)
{
	if(!this->isBound(key)) this->keybindings[key] = std::vector<Action>{};
	this->keybindings[key].push_back(action);
}

void PlayerComponent::unbind(Key key)
{
	if(this->isBound(key)) this->keybindings.erase(key);
}

void PlayerComponent::onKeyPress(Key key)
{
	if(this->isBound(key)) for(auto const &action : this->keybindings[key]) action(true);
}

void PlayerComponent::onKeyRelease(Key key)
{
	if(this->isBound(key)) for(auto const &action : this->keybindings[key]) action(false);
}

bool PlayerComponent::isBound(Key key)
{
	return this->keybindings.find(key) != this->keybindings.end();
}

void HealthComponent::damage(float amount)
{
	if(this->health - amount < 0.0f) this->health = 0.0f;
	else this->health -= amount;
}

void HealthComponent::heal(float amount)
{
	if(this->health + amount > this->maxHealth) this->health = this->maxHealth;
	else this->health += amount;
}

aabb2D<double> CollisionComponent::getBoundingBox(UP<SpatialComponent> const &spatialComp)
{
	vec2<double> halfScale{std::abs(spatialComp->scale.x() / 2.0), std::abs(spatialComp->scale.y() / 2.0)};
	return aabb2D<double>(spatialComp->pos.x() - halfScale.x(), spatialComp->pos.x() + halfScale.x(), spatialComp->pos.y() - halfScale.y(), spatialComp->pos.y() + halfScale.y(), spatialComp->pos.x(), spatialComp->pos.y());
}

void ScriptComponent::run(std::string const &scriptName)
{
	//TODO scripting language, AssetRepository for compiled scripts, JIT compiler
	if(this->scripts.find(scriptName) != this->scripts.end())
	{
		
	}
	else if(this->scripts.find(scriptName) != this->scripts.end())
	{
		
	}
}
