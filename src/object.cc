#include "object.hh"

void Object::serialize(Serializer &serializer)
{
	if(this->spatialComp)
	{
		serializer.write(this->spatialComp->pos);
		serializer.write(this->spatialComp->rotation);
		serializer.write(this->spatialComp->scale);
	}
	
}

void Object::deserialize(Serializer &serializer)
{
	if(this->spatialComp)
	{
		serializer.readContainer(this->spatialComp->pos);
		serializer.read(this->spatialComp->rotation);
		serializer.readContainer(this->spatialComp->scale);
	}
	
}

SP<Object> Object::copy()
{
	SP<Object> out = Object::create(this->name);
	if(this->playerComp)
	{
		out->addPlayerComp();
		out->playerComp->keybindings = this->playerComp->keybindings;
	}
	if(this->entityComp)
	{
		out->addEntityComp();
	}
	if(this->healthComp)
	{
		out->addHealthComp();
		out->healthComp->health = this->healthComp->health;
		out->healthComp->maxHealth = this->healthComp->maxHealth;
	}
	if(this->spatialComp)
	{
		out->addSpatialComp();
		out->spatialComp->pos = this->spatialComp->pos;
		out->spatialComp->rotation = this->spatialComp->rotation;
		out->spatialComp->scale = this->spatialComp->scale;
		out->spatialComp->velocity = this->spatialComp->velocity;
		out->spatialComp->moveSpeed = this->spatialComp->moveSpeed;
		out->spatialComp->affectedByGravity = this->spatialComp->affectedByGravity;
		out->spatialComp->onGround = this->spatialComp->onGround;
	}
	if(this->collisionComp)
	{
		out->addCollisionComp();
		out->collisionComp->canCollide = this->collisionComp->canCollide;
		out->collisionComp->canMove = this->collisionComp->canMove;
	}
	if(this->graphicsComp)
	{
		out->addGraphicsComp();
		out->graphicsComp->layer = this->graphicsComp->layer;
		out->graphicsComp->subLayer = this->graphicsComp->subLayer;
	}
	if(this->soundComp)
	{
		out->addSoundComp();
	}
	if(this->aiComp)
	{
		out->addAIComp();
	}
	if(this->textComp)
	{
		out->addTextComp();
		out->textComp->text = this->textComp->text;
	}
	if(this->scriptComp)
	{
		out->addScriptComp();
		out->scriptComp->scripts = this->scriptComp->scripts;
		out->scriptComp->compiledScripts = this->scriptComp->compiledScripts;
	}
	return out;
}

SP<Object> Object::copyWithOffset(float x, float y)
{
	SP<Object> out = Object::create(this->name);
	if(this->playerComp)
	{
		out->addPlayerComp();
		out->playerComp->keybindings = this->playerComp->keybindings;
	}
	if(this->entityComp)
	{
		out->addEntityComp();
	}
	if(this->healthComp)
	{
		out->addHealthComp();
		out->healthComp->health = this->healthComp->health;
		out->healthComp->maxHealth = this->healthComp->maxHealth;
	}
	if(this->spatialComp)
	{
		out->addSpatialComp();
		out->spatialComp->pos = this->spatialComp->pos;
		out->spatialComp->pos += {x, y};
		out->spatialComp->rotation = this->spatialComp->rotation;
		out->spatialComp->scale = this->spatialComp->scale;
		out->spatialComp->velocity = this->spatialComp->velocity;
		out->spatialComp->moveSpeed = this->spatialComp->moveSpeed;
		out->spatialComp->affectedByGravity = this->spatialComp->affectedByGravity;
		out->spatialComp->onGround = this->spatialComp->onGround;
	}
	if(this->collisionComp)
	{
		out->addCollisionComp();
		out->collisionComp->canCollide = this->collisionComp->canCollide;
		out->collisionComp->canMove = this->collisionComp->canMove;
	}
	if(this->graphicsComp)
	{
		out->addGraphicsComp();
		out->graphicsComp->atlasID = this->graphicsComp->atlasID;
		out->graphicsComp->name = this->graphicsComp->name;
		out->graphicsComp->layer = this->graphicsComp->layer;
		out->graphicsComp->subLayer = this->graphicsComp->subLayer;
	}
	if(this->soundComp)
	{
		out->addSoundComp();
	}
	if(this->aiComp)
	{
		out->addAIComp();
	}
	if(this->textComp)
	{
		out->addTextComp();
		out->textComp->text = this->textComp->text;
	}
	if(this->scriptComp)
	{
		out->addScriptComp();
		out->scriptComp->scripts = this->scriptComp->scripts;
		out->scriptComp->compiledScripts = this->scriptComp->compiledScripts;
	}
	return out;
}
