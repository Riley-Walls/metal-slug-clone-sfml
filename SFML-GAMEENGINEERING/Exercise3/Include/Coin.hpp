#pragma once
#include <Book/Entity.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>

#include "Box2D\Box2D.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <InteractableObject.hpp>
#include <B2SpriteNode.hpp>
#include <Hero.hpp>

class Coin : public B2SpriteNode
{

public:
	Coin(b2World& mB2World, float xPos, float yPos, const TextureHolder& textures, Textures::ID textureID, b2BodyType bodyType, bool hasFixedRotation = false);
	
	void	SetTarget(Hero* hero);
	void	updateCurrent(sf::Time dt);
	void	PickUp();

private:

	Hero*	mTargetHero;

};

