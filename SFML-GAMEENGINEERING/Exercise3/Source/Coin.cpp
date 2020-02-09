#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Coin.hpp>
#include <Book/World.hpp>

#include <iostream>


Coin::Coin(b2World& mB2World, float xPos, float yPos, const TextureHolder& textures, Textures::ID textureID, b2BodyType bodyType, bool hasFixedRotation)
	: B2SpriteNode(mB2World, xPos, yPos, textures, textureID, bodyType, hasFixedRotation)
	, mTargetHero(nullptr)
{
	B2SpriteNode::SetCollisionBits(World::B2CollisionBits::COIN_BIT);
	SetCollisionsEnabled(false);
}

void Coin::SetTarget(Hero* hero)
{
	mTargetHero = hero;
}

void Coin::updateCurrent(sf::Time dt) {
	B2SpriteNode::updateCurrent(dt);
}

void Coin::PickUp()
{
	mTargetHero->AddCoin();
	SetEnabled(false);
}