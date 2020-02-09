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

#include <SFML/System/Time.hpp>

class Explosion : public SceneNode
{

public:
	Explosion(b2World& mB2World, const TextureHolder& textures, float x, float y);
	void	updateCurrent(sf::Time dt);
	bool	isTouchingPlayer = false;

	void	Explode();

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Clock				animClock;
	sf::IntRect				rectSourceSprite;

	sf::RectangleShape		mDebugCube;
	sf::Sprite				mSprite;

	bool				mAnimationWasTriggered = false;
};

