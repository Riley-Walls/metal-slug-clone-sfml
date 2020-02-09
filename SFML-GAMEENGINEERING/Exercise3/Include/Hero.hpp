#pragma once
#include <Book/Entity.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include "SFML/Graphics.hpp"


#include "Box2D\Box2D.h"
#include <SFML/Graphics/RectangleShape.hpp>

#include <SFML/System/Time.hpp>

class Hero : public SceneNode
{

public:
	enum HeroState
	{
		STANDING,
		JUMPING,
		FALLING,
		RUNNING,
	};

	enum Weapons {
		PISTOL,
		GRENADELAUNCHER,
		WEAPONCOUNT,
	};

	Hero(b2World& mB2World, TextureHolder& textures, SceneNode* parentNode);
	virtual unsigned int	getCategory() const;
	virtual void		updateCurrent(sf::Time dt);

	b2Body* mB2Body;
	b2Fixture* mB2BodyFixture;

	int						mCollisionCount = 0;

	int						mHealth = 100;
	int						mCoinCount = 0;

	void					Jump();
	void					Damage(int amt);
	void					Heal(int amt);
	void					AddCoin();
	void					Shoot();
	void					CreateBullet();
	void					CreateGrenade();
	void					CycleWeapon();

private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	HeroState				getState();
	HeroState				currentState = STANDING;
	HeroState				previousState = STANDING;
	bool					isFacingRight = true;
	bool					mJumpWasTriggered = false;
	float					mDefaultFriction = 2.f;

	sf::Clock				animClock;
	sf::IntRect				rectSourceSpriteLegs;
	sf::IntRect				rectSourceSpriteUpper;
	int						animFrameCount;

	b2World&				mB2World;
	TextureHolder&			mTextures;

	void					setWeapon(Weapons weapon);
	Weapons					mCurrentWeapon;

	float	mWeaponCooldownLength = .2f;
	float   mWeaponCooldownTimer = .0f;

	bool	bulletWasSpawned = false;
	SceneNode* parentNode;

private:
	sf::Sprite				mLegSprite;
	sf::Sprite				mUpperBodySprite;
	sf::RectangleShape		mDebugCube;

};

