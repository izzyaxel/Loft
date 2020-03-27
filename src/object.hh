#pragma once

#include "assets.hh"
#include "components.hh"

#include <commons/serialization.hh>
#include <commons/math/vec2.hh>
#include <commons/math/shapes.hh>
#include <cstdint>
#include <string>

struct Object : public Serializable
{
	[[nodiscard]] static inline bool comparator(SP<Object> const &a, SP<Object> const &b)
	{
		return (a->graphicsComp && b->graphicsComp) && (a->graphicsComp->layer == b->graphicsComp->layer) ? a->graphicsComp->subLayer > b->graphicsComp->subLayer : a->graphicsComp->layer > b->graphicsComp->layer;
	}
	
	void serialize(Serializer &serializer) override;
	void deserialize(Serializer &serializer) override;
	
	[[nodiscard]] SP<Object> copy();
	[[nodiscard]] SP<Object> copyWithOffset(float x, float y);
	
	Object(std::string const &name)
	{
		this->name = name;
	}
	
	[[nodiscard]] static SP<Object> create(std::string const &name)
	{
		return MS<Object>(name);
	}
	
	inline void addPlayerComp()
	{
		this->playerComp = MU<PlayerComponent>();
	}
	
	inline void addEntityComp()
	{
		this->entityComp = MU<EntityComponent>();
	}
	
	inline void addHealthComp()
	{
		this->healthComp = MU<HealthComponent>();
	}
	
	inline void addSpatialComp()
	{
		this->spatialComp = MU<SpatialComponent>();
	}
	
	inline void addCollisionComp()
	{
		this->collisionComp = MU<CollisionComponent>();
	}
	
	inline void addGraphicsComp()
	{
		this->graphicsComp = MU<GraphicsComponent>();
	}
	
	inline void addSoundComp()
	{
		this->soundComp = MU<SoundComponent>();
	}
	
	inline void addAIComp()
	{
		this->aiComp = MU<AIComponent>();
	}
	
	inline void addTextComp()
	{
		this->textComp = MU<TextComponent>();
	}
	
	inline void addScriptComp()
	{
		this->scriptComp = MU<ScriptComponent>();
	}
	
	using UpdateHandler = std::function<void (SP<Object> const &obj, double delta)>;
	inline void registerUpdateHandler(UpdateHandler const &handler)
	{
		this->updateHandlers.push_back(handler);
	}
	
	std::vector<UpdateHandler> updateHandlers{};
	
	std::string name = "";
	
	UP<PlayerComponent> playerComp = nullptr;
	UP<EntityComponent> entityComp = nullptr;
	UP<HealthComponent> healthComp = nullptr;
	UP<SpatialComponent> spatialComp = nullptr;
	UP<CollisionComponent> collisionComp = nullptr;
	UP<GraphicsComponent> graphicsComp = nullptr;
	UP<SoundComponent> soundComp = nullptr;
	UP<AIComponent> aiComp = nullptr;
	UP<TextComponent> textComp = nullptr;
	UP<ScriptComponent> scriptComp = nullptr;
};
