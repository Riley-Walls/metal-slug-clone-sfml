#pragma once
#include <Book/Entity.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>

#include "Box2D\Box2D.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <InteractableObject.hpp>

class B2SpriteNode : public Entity
{

public:
	B2SpriteNode(b2World& mB2World, float xPos, float yPos, const TextureHolder& textures, Textures::ID textureID, b2BodyType bodyType, bool hasFixedRotation = false);
	virtual unsigned int	getCategory() const;
	virtual void			updateCurrent(sf::Time dt);

	b2Body* mB2Body;

	void					SetCollisionsEnabled(bool tf);
	void					SetCollisionBits(uint16 collisionMask);
	void					SetEnabled(bool tf);

	void					SetFriction(float value);

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void					RedefineB2Body();

	bool					isEnabled = true;
	bool					mSetToDisabled = false;

protected:
	sf::RectangleShape		mDebugCube;
	sf::Sprite				mSprite;
	b2World&				mB2World;
	uint16					mCollisionBits;
	bool					mTimeToRedefineB2Body = false;


};

