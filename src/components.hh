#pragma once

#include "def.hh"
#include "input.hh"

#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>
#include <commons/math/vec2.hh>
#include <commons/math/shapes.hh>

struct Object;

/// Depends on SpatialComponent
struct PlayerComponent
{
	//Paramter is keyDown
	using Action = std::function<void (bool)>;
	
	void bind(Key key, Action action);
	void unbind(Key key);
	void onKeyPress(Key key);
	void onKeyRelease(Key key);
	[[nodiscard]] bool isBound(Key key);
	
	std::unordered_map<Key, std::vector<Action>> keybindings;
};

/// Defines an object as an entity
struct EntityComponent
{
	
};

struct HealthComponent
{
	void damage(float amount);
	void heal(float amount);
	
	float health = 1.0f, maxHealth = 1.0f;
};

struct SpatialComponent
{
	vec2<double> pos{}, scale{}, velocity{};
	double rotation = 0.0f, moveSpeed = 0.0f;
	bool affectedByGravity = true, onGround = false;
};

/// Depends on SpatialComponent
struct CollisionComponent
{
	[[nodiscard]] aabb2D<double> getBoundingBox(UP<SpatialComponent> const &spatialComp);
	
	bool canCollide = true, canMove = true;
};

/// Depends on SpatialComponent
struct GraphicsComponent
{
	using AnimationHandler = std::function<void(SP<Object> const &obj, double delta)>;
	
	uint64_t atlasID = 0, shaderID = 0;
	std::string name = "";
	
	uint64_t layer = 0, subLayer = 0;
	
	AnimationHandler animationHandler;
};

struct SoundComponent
{
	
};

/// Mututally exclusive with PlayerComponent, depends on SpatialComponent
struct AIComponent
{
	
};

struct TextComponent
{
	vec2<double> pos{}, scale{}; //If hudText is true, the origin is the upper-left corner of the window, if false the world's origin is the origin
	double rotation = 0.0f;
	uint32_t width = 0, height = 0, x = 0, y = 0;
	std::string text = "";
	uint32_t texID = 0; //Don't modify this, it's handled internally
};

struct ScriptComponent
{
	void run(std::string const &scriptName);
	
	std::unordered_map<std::string, std::string> scripts;
	std::unordered_map<std::string, uint64_t> compiledScripts;
};
