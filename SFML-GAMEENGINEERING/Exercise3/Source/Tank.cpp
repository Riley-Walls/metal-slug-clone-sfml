#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Tank.hpp>
#include <Hero.hpp>
#include <Book/World.hpp>
#include <iostream>

#include <TankBullet.hpp>
#include <Bullet.hpp>
#include <Grenade.hpp>

#include <cmath>


Tank::Tank(TankType tankType, b2World& mB2World, TextureHolder& textures, float posX, float posY, Hero* hero, SceneNode* parentNode)
	: mSprite()
	, mB2Body(nullptr)
	, mB2World(mB2World)
	, mTextures(textures)
	, parentNode(parentNode)
	, mHero(hero)
{
	if (tankType == TankType::BULLET) {
		mSprite.setTexture(textures.get(Textures::Tank));
		mWeaponCooldownLength = 1.7f;
	}
	else if (tankType == TankType::MISSILE) {
		mSprite.setTexture(textures.get(Textures::MissileTank));
		mWeaponCooldownLength = 5.f;
	}

	mWeaponCooldownTimer = mWeaponCooldownLength;

	mTankType = tankType;

	float bodyWidth = mSprite.getGlobalBounds().width;
	float bodyHeight = mSprite.getGlobalBounds().height;
	//int bodyWidth = 120;
	//int bodyHeight = 112;

	/*
	//Offset sprite
	centerOrigin(mSprite);
	float newLegsX = mSprite.getPosition().x;
	float newLegsY = mSprite.getPosition().y + 25;
	mSprite.setPosition(newLegsX, newLegsY);
	*/

	//Debug Cube
	sf::Color cubeColor(255, 0, 0, 100);
	mDebugCube.setSize(sf::Vector2f(bodyWidth, bodyHeight));
	mDebugCube.setFillColor(cubeColor);

	//Define Physics Body
	b2BodyDef BodyDefCube;
	BodyDefCube.position = b2Vec2(posX / World::SCALE, posY / World::SCALE);
	BodyDefCube.type = b2_dynamicBody;
	BodyDefCube.fixedRotation = true;
	BodyDefCube.userData = this;
	mB2Body = mB2World.CreateBody(&BodyDefCube);

	b2PolygonShape CubeShape;
	CubeShape.SetAsBox((bodyWidth / 2) / World::SCALE, (bodyHeight / 2) / World::SCALE);
	b2FixtureDef CubeFixtureDef;
	CubeFixtureDef.density = 1.5f;
	CubeFixtureDef.shape = &CubeShape;
	CubeFixtureDef.friction = mDefaultFriction;
	CubeFixtureDef.filter.categoryBits = World::B2CollisionBits::TANK_BIT;
	mB2BodyFixture = mB2Body->CreateFixture(&CubeFixtureDef);

	mDebugCube.setOrigin(bodyWidth / 2, bodyHeight / 2);
	
}

void Tank::Destroy() {
	setToDestroy = true;
}

void Tank::Shoot() {
	if (mWeaponCooldownTimer >= mWeaponCooldownLength) {
		if (mTankType == TankType::BULLET) {
			CreateBullet();
		}
		else if (mTankType == TankType::MISSILE) {
			CreateGrenade();
		}
		mWeaponCooldownTimer = 0.f;
	}
}

void Tank::CreateBullet() {
	if (mWeaponCooldownTimer >= mWeaponCooldownLength) {
		std::cout << "Creating Bullet\n";

		float bulletSpawnX = 0;
		if (isFacingRight) {
			bulletSpawnX = getPosition().x + (mSprite.getGlobalBounds().width / 2) + 20;
		}
		else {
			bulletSpawnX = getPosition().x - (mSprite.getGlobalBounds().width / 2) - 20;
		}


		std::unique_ptr<TankBullet> BulletPtr(new TankBullet(mB2World, mTextures, bulletSpawnX, getPosition().y));
		TankBullet* bullet = BulletPtr.get();

		bullet->Shoot(isFacingRight);

		/*
		if (isFacingRight) {
			bullet->SetVelocity(10, 0);
			std::cout << "Is facing right\n";
		}
		else {
			bullet->SetVelocity(-10, 0);
			std::cout << "Is facing left\n";
		}
		*/
		
		parentNode->attachChild(std::move(BulletPtr));
		std::cout << "Bullet Created\n";

	}

}

void Tank::CreateGrenade() {

	if (mWeaponCooldownTimer >= mWeaponCooldownLength) {
		std::cout << "Creating Grenade\n";

		float bulletSpawnX = 0;
		if (isFacingRight) {
			bulletSpawnX = getPosition().x + (mSprite.getGlobalBounds().width / 2) + 30;
		}
		else {
			bulletSpawnX = getPosition().x - (mSprite.getGlobalBounds().width / 2) - 30;
		}

		std::unique_ptr<Grenade> GrenadePtr(new Grenade(mB2World, mTextures, bulletSpawnX, getPosition().y));
		Grenade* bullet = GrenadePtr.get();
		bullet->mGrenadeType = Grenade::GrenadeType::ENEMY;

		if (isFacingRight) {
			bullet->SetVelocity(2, 0);
			std::cout << "Is facing right\n";
		}
		else {
			bullet->SetVelocity(-2, 0);
			std::cout << "Is facing left\n";
		}

		parentNode->attachChild(std::move(GrenadePtr));
		std::cout << "Grenade Created\n";

	}
}

void Tank::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!setToDestroy && !destroyed) {
		target.draw(mSprite, states);
		if (World::DebugShowBounds) {
			target.draw(mDebugCube, states);
		}
	}
}

unsigned int Tank::getCategory() const
{
	return Category::None;
}

Tank::HeroState Tank::getState() {

	//CHECK THAT PLAYER IS STILL JUMPING
	//If jump was triggered, but now Y velocity is close to zero and player is colliding with something, turn off jump
	if (((mB2Body->GetLinearVelocity().y * -1.0f) < 1.f && currentState == JUMPING)
		&& ((mB2Body->GetLinearVelocity().y * -1.0f) > -1.f && previousState == JUMPING)
		&& mJumpWasTriggered
		&& mCollisionCount > 0) {

		mJumpWasTriggered = false;
	}
	if (mJumpWasTriggered) {
		return JUMPING;
	}
	if ((mB2Body->GetLinearVelocity().y * -1.0f) < -1.f) {
		return FALLING;
	}
	else if (mB2Body->GetLinearVelocity().x > 1.f || mB2Body->GetLinearVelocity().x < -1.f) {
		return RUNNING;
	}
	else {
		return STANDING;
	}

	/*
	if (((mB2Body->GetLinearVelocity().y * -1.0f) > .0001f && currentState == JUMPING)
		|| ((mB2Body->GetLinearVelocity().y * -1.0f) < -.0001f && previousState == JUMPING)) {
		return JUMPING;
	}
	else if ((mB2Body->GetLinearVelocity().y * -1.0f) < -.0001f) {
		return FALLING;
	}
	else if (mB2Body->GetLinearVelocity().x > 0.0001f || mB2Body->GetLinearVelocity().x < -0.0001f) {
		return RUNNING;
	}
	else {
		return STANDING;
	}
	*/
}

void Tank::Jump() {

	if (currentState != JUMPING) {
		mJumpWasTriggered = true;
		mB2Body->ApplyLinearImpulse(b2Vec2(0, -80), mB2Body->GetWorldCenter(), true);
		currentState = JUMPING;
	}
}

void Tank::Damage(int amt) {

	mHealth -= amt;
	if (mHealth < 0) {
		mHealth = 0;
	}
}

void Tank::Heal(int amt) {

	mHealth += amt;
	if (mHealth > 100) {
		mHealth = 100;
	}
}

void Tank::AddCoin() {
	mCoinCount++;
}

void Tank::updateCurrent(sf::Time dt)
{
	if (setToDestroy && !destroyed) {
		std::cout << "\nDestroy Body\n\n";
		mB2World.DestroyBody(mB2Body);
		destroyed = true;
	}
	else if (!setToDestroy && !destroyed) {
		//Weapon cooldown
		mWeaponCooldownTimer += dt.asSeconds();


		float heroDetectRange = 0;
		float firingRange = 0;
		float stopRange = 0;

		if (mTankType == TankType::BULLET) {
			heroDetectRange = 500;
			firingRange = 300;
			stopRange = 300;
		}
		else if (mTankType == TankType::MISSILE) {
			heroDetectRange = 600;
			firingRange = 500;
			stopRange = 300;
		}


		float distanceFromHero = mHero->getPosition().x - getPosition().x;

		if (std::abs(distanceFromHero) < heroDetectRange && std::abs(distanceFromHero) > stopRange) {

			float velX = 0;
			float velY = 0;

			if (mTankType == TankType::BULLET) {
				velX = 13;
				velY = -5;
			}
			else if (mTankType == TankType::MISSILE) {
				velX = 20;
				velY = -10;
			}

			if (distanceFromHero < 0) {
				velX = velX * -1;
			}

			if (mB2Body->GetLinearVelocity().x <= 1.5 && mB2Body->GetLinearVelocity().x >= -1.5) {
				mB2Body->ApplyLinearImpulse(b2Vec2(velX, velY), mB2Body->GetWorldCenter(), true);
			}
		}
		if (std::abs(distanceFromHero) < firingRange) {
			std::cout << "Shooting!\n";
			Shoot();
		}


		float spriteWidth = mSprite.getGlobalBounds().width;
		float spriteHeight = mSprite.getGlobalBounds().height;

		previousState = currentState;
		currentState = getState();

		if (isFacingRight && mB2Body->GetLinearVelocity().x < -0.5f || isFacingRight && distanceFromHero < 0) {
			isFacingRight = false;
			mSprite.scale(-1.f, 1);
		}
		if (!isFacingRight && mB2Body->GetLinearVelocity().x > 0.5f || !isFacingRight && distanceFromHero > 0) {
			isFacingRight = true;
			mSprite.scale(-1.f, 1);
		}

		/* //DEBUG MESSAGES
		switch (currentState){
		case STANDING: std::cout << "STANDING\n";
			break;
		case JUMPING: std::cout << "JUMPING\n";
			break;
		case FALLING: std::cout << "FALLING\n";
			break;
		case RUNNING: std::cout << "RUNNING\n";
			break;
		}
		*/

		//Center Debug Cube
		//mDebugCube.setOrigin(spriteWidth, spriteHeight);
		//Center Sprite
		centerOrigin(mSprite);

		//centerOrigin(mUpperBodySprite);
		//Offset Upper Body Sprite
		/*
		float newUpperBodyX = mUpperBodySprite.getPosition().x;
		float newUpperBodyY = mUpperBodySprite.getPosition().y - 25;
		mUpperBodySprite.setPosition(newUpperBodyX, newUpperBodyY);
		*/

		//Get Position & Rotation from B2Body
		float newX = mB2Body->GetPosition().x * World::SCALE;
		float newY = mB2Body->GetPosition().y * World::SCALE;
		setPosition(newX, newY);
		float newRot = (mB2Body->GetAngle() * 180 / b2_pi);
		setRotation(newRot);



	}


	
}