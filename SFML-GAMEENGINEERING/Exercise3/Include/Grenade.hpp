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

class Grenade : public SceneNode
{

public:
	Grenade(b2World& mB2World, const TextureHolder& textures, float x, float y);
	void	SetTarget(SceneNode* newTarget);
	void	updateCurrent(sf::Time dt);
	bool	isTouchingPlayer = false;

	void	SetVelocity(int x, int y);
	void	Destroy();

	enum GrenadeType {
		HERO,
		ENEMY,
	};

	GrenadeType mGrenadeType;

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

	Hero*	mTargetHero;
	float const mAttackCooldown = 1;
	float mAttackCooldownTimer = 1;

	sf::Clock				animClock;
	sf::IntRect				rectSourceSprite;

	sf::RectangleShape		mDebugCube;
	sf::Sprite				mSprite;

	b2World&				mB2World;
	b2Body*					mB2Body;
	b2Fixture*				mB2BodyFixture;

	//Circle Sensor
	sf::CircleShape		mDebugCircle;
	b2Fixture*			mB2CircleSensorFixture;
	SceneNode*			mCurrentTarget;



	bool	setToDestroy = false;
	bool	destroyed = false;

	void	attackCurrentTarget();
	bool				mAnimationWasTriggered = false;
	bool				mIdleAnimIsReversed = false;

	sf::Vector2f		mVelocity;
	bool				isFacingRight;

	void				guideTowards(sf::Vector2f position);
	sf::Vector2f		mTargetDirection;
};

