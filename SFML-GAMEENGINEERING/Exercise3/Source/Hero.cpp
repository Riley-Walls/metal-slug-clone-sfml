#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Hero.hpp>
#include <Book/World.hpp>
#include <iostream>

#include <Bullet.hpp>
#include <Grenade.hpp>


Hero::Hero(b2World& mB2World, TextureHolder& textures, SceneNode* parentNode)
	: rectSourceSpriteLegs(0, 0, 62, 40)
	, rectSourceSpriteUpper(0, 0, 74, 54)
	, mLegSprite(textures.get(Textures::Hero_Run_Legs), rectSourceSpriteLegs)
	, mUpperBodySprite(textures.get(Textures::Hero_Upper_Body_Weapons), rectSourceSpriteUpper)
	//, mUpperBodySpriteGrenadeLauncher(textures.get(Textures::Hero_Upper_Body_Grenade_Launcher))
	, mB2Body(nullptr)
	, mB2World(mB2World)
	, mTextures(textures)
	, parentNode(parentNode)
{
	setWeapon(Weapons::PISTOL);

	//float spriteWidth = mLegSprite.getGlobalBounds().width;
	//float spriteHeight = mLegSprite.getGlobalBounds().height;

	int bodyWidth = 45;
	int bodyHeight = 77;


	//Offset legs
	centerOrigin(mLegSprite);
	float newLegsX = mLegSprite.getPosition().x;
	float newLegsY = mLegSprite.getPosition().y + 25;
	mLegSprite.setPosition(newLegsX, newLegsY);

	//Offset upper body
	centerOrigin(mUpperBodySprite);
	float newUpperBodyX = mLegSprite.getPosition().x + 12;
	float newUpperBodyY = mUpperBodySprite.getPosition().y - 10;
	mUpperBodySprite.setPosition(newUpperBodyX, newUpperBodyY);

	//Debug Cube
	sf::Color cubeColor(255, 0, 0, 100);
	mDebugCube.setSize(sf::Vector2f(bodyWidth, bodyHeight));
	mDebugCube.setFillColor(cubeColor);

	//Define Physics Body
	b2BodyDef BodyDefCube;
	BodyDefCube.position = b2Vec2(800 / World::SCALE, 500 / World::SCALE);
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
	CubeFixtureDef.filter.categoryBits = World::B2CollisionBits::HERO_BIT;
	mB2BodyFixture = mB2Body->CreateFixture(&CubeFixtureDef);

	mDebugCube.setOrigin(bodyWidth / 2, bodyHeight / 2);
	
}


void Hero::setWeapon(Weapons weapon) {
	mCurrentWeapon = weapon;
	int weaponIndex = weapon;

	rectSourceSpriteUpper.left = rectSourceSpriteUpper.width * weaponIndex;
	std::cout << "rectSourceSpriteUpper.left: " + std::to_string(rectSourceSpriteUpper.width * weaponIndex) + "\n";
	mUpperBodySprite.setTextureRect(rectSourceSpriteUpper);

	if (weapon == Weapons::PISTOL) {
		std::cout << "Swapping to Pistol\n";
	}
	else if (weapon == Weapons::GRENADELAUNCHER){
		std::cout << "Swapping to Grenade Launcher\n";
	}
}

void Hero::CycleWeapon() {
	int currentWeaponIndex = mCurrentWeapon;
	int newWeaponIndex = currentWeaponIndex + 1;
	newWeaponIndex = newWeaponIndex % Weapons::WEAPONCOUNT;
	
	setWeapon((Weapons)newWeaponIndex);
}

void Hero::Shoot() {
	switch (mCurrentWeapon) {
	case Weapons::PISTOL: 
		CreateBullet();
		break;
	case Weapons::GRENADELAUNCHER:
		CreateGrenade();
		break;
	default: break;
	}
}

void Hero::CreateBullet() {
	if (mWeaponCooldownTimer >= mWeaponCooldownLength) {
		std::cout << "Creating Bullet\n";

		float bulletSpawnX = 0;
		if (isFacingRight) {
			bulletSpawnX = getPosition().x + 50;
		}
		else {
			bulletSpawnX = getPosition().x - 50;
		}


		std::unique_ptr<Bullet> BulletPtr(new Bullet(mB2World, mTextures, bulletSpawnX, getPosition().y));
		Bullet* bullet = BulletPtr.get();

		if (isFacingRight) {
			bullet->SetVelocity(10, 0);
			std::cout << "Is facing right\n";
		}
		else {
			bullet->SetVelocity(-10, 0);
			std::cout << "Is facing left\n";
		}
		
		parentNode->attachChild(std::move(BulletPtr));
		std::cout << "Bullet Created\n";

		mWeaponCooldownTimer = 0;
	}

}

void Hero::CreateGrenade() {

	if (mWeaponCooldownTimer >= mWeaponCooldownLength) {
		std::cout << "Creating Grenade\n";

		float bulletSpawnX = 0;
		if (isFacingRight) {
			bulletSpawnX = getPosition().x + 80;
		}
		else {
			bulletSpawnX = getPosition().x - 80;
		}

		std::unique_ptr<Grenade> GrenadePtr(new Grenade(mB2World, mTextures, bulletSpawnX, getPosition().y));
		Grenade* bullet = GrenadePtr.get();

		if (isFacingRight) {
			bullet->SetVelocity(2, 0);
			std::cout << "Is facing right\n";
		}
		else {
			bullet->SetVelocity(-2, 0);
			std::cout << "Is facing left\n";
		}

		parentNode->attachChild(std::move(GrenadePtr));
		std::cout << "Bullet Created\n";

		mWeaponCooldownTimer = 0;
	}
}

void Hero::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mLegSprite, states);
	target.draw(mUpperBodySprite, states);
	if (World::DebugShowBounds) {
		target.draw(mDebugCube, states);
	}
}

unsigned int Hero::getCategory() const
{
	return Category::Hero;
}

Hero::HeroState Hero::getState() {

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

void Hero::Jump() {

	if (currentState != JUMPING) {
		mJumpWasTriggered = true;
		mB2Body->ApplyLinearImpulse(b2Vec2(0, -80), mB2Body->GetWorldCenter(), true);
		currentState = JUMPING;
	}
}

void Hero::Damage(int amt) {

	mHealth -= amt;
	if (mHealth < 0) {
		mHealth = 0;
	}
}

void Hero::Heal(int amt) {

	mHealth += amt;
	if (mHealth > 100) {
		mHealth = 100;
	}
}

void Hero::AddCoin() {
	mCoinCount++;
}

void Hero::updateCurrent(sf::Time dt)
{
	float spriteWidth = mLegSprite.getGlobalBounds().width;
	float spriteHeight = mLegSprite.getGlobalBounds().height;

	previousState = currentState;
	currentState = getState();


		if (isFacingRight && mB2Body->GetLinearVelocity().x < -0.5f) {
			isFacingRight = false;
			mLegSprite.scale(-1.f, 1);

			mUpperBodySprite.scale(-1.f, 1);
			float newUpperBodyX = mLegSprite.getPosition().x - 12;
			float newUpperBodyY = mUpperBodySprite.getPosition().y;
			mUpperBodySprite.setPosition(newUpperBodyX, newUpperBodyY);
		}
		if (!isFacingRight && mB2Body->GetLinearVelocity().x > 0.5f) {
			isFacingRight = true;
			mLegSprite.scale(-1.f, 1);

			mUpperBodySprite.scale(-1.f, 1);
			float newUpperBodyX = mLegSprite.getPosition().x + 12;
			float newUpperBodyY = mUpperBodySprite.getPosition().y;
			mUpperBodySprite.setPosition(newUpperBodyX, newUpperBodyY);
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
	centerOrigin(mLegSprite);

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

	//ANIMATION ------------------------------------------

	int width = 62;
	int frameCount = 11;
	float timePerFrame = 1.0f;

	if (currentState == RUNNING) {
		if (animClock.getElapsedTime().asSeconds() > .05f) {

			if (rectSourceSpriteLegs.left == width * frameCount)
				rectSourceSpriteLegs.left = 0;
			else
				rectSourceSpriteLegs.left += width;

			mLegSprite.setTextureRect(rectSourceSpriteLegs);
			animClock.restart();
		}
	}
	else if (currentState == JUMPING) {
		rectSourceSpriteLegs.left = width * 2;
		mLegSprite.setTextureRect(rectSourceSpriteLegs);
	}
	else if (currentState == STANDING) {
		rectSourceSpriteLegs.left = width * 5;
		mLegSprite.setTextureRect(rectSourceSpriteLegs);
	}

	//Weapon cooldown
	mWeaponCooldownTimer += dt.asSeconds();
}