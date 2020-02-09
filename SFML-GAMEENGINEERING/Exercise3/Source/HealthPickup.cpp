#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <HealthPickup.hpp>
#include <Book/World.hpp>

#include <iostream>


HealthPickup::HealthPickup(b2World& mB2World, float xPos, float yPos, const TextureHolder& textures, Textures::ID textureID, b2BodyType bodyType, bool hasFixedRotation)
	: B2SpriteNode(mB2World, xPos, yPos, textures, textureID, bodyType, hasFixedRotation)
	, mTargetHero(nullptr)
{
	B2SpriteNode::SetCollisionBits(World::B2CollisionBits::HEALTH_PICKUP_BIT);
}

void HealthPickup::SetTarget(Hero* hero)
{
	mTargetHero = hero;
}

void HealthPickup::updateCurrent(sf::Time dt) {
	B2SpriteNode::updateCurrent(dt);
}

void HealthPickup::PickUp()
{
	mTargetHero->Heal(50);
	SetEnabled(false);
}