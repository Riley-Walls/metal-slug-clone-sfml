#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Snake.hpp>
#include <Book/World.hpp>

#include <iostream>


Snake::Snake(b2World& mB2World, const TextureHolder& textures, float x, float y)
	: rectSourceSprite(0, 0, 66, 54)
	, mSprite(textures.get(Textures::Snake_Anim), rectSourceSprite)
	, mB2Body(nullptr)
{
	//float spriteWidth = mLegSprite.getGlobalBounds().width;
	//float spriteHeight = mLegSprite.getGlobalBounds().height;

	int bodyWidth = 66;
	int bodyHeight = 54;

	/*
	//Offset upper body
	centerOrigin(mUpperBodySprite);
	float newUpperBodyX = mUpperBodySprite.getPosition().x;
	float newUpperBodyY = mUpperBodySprite.getPosition().y - 10;
	mUpperBodySprite.setPosition(newUpperBodyX, newUpperBodyY);

	//Offset legs
	centerOrigin(mLegSprite);
	float newLegsX = mLegSprite.getPosition().x;
	float newLegsY = mLegSprite.getPosition().y + 25;
	mLegSprite.setPosition(newLegsX, newLegsY);
	*/

	//Debug Cube
	sf::Color cubeColor(255, 0, 0, 100);
	mDebugCube.setSize(sf::Vector2f(bodyWidth, bodyHeight));
	mDebugCube.setFillColor(cubeColor);

	//Define Physics Body
	b2BodyDef BodyDefCube;
	BodyDefCube.position = b2Vec2(x / World::SCALE, y / World::SCALE);
	BodyDefCube.type = b2_staticBody;
	BodyDefCube.fixedRotation = true;
	BodyDefCube.userData = this;
	mB2Body = mB2World.CreateBody(&BodyDefCube);

	b2PolygonShape CubeShape;
	CubeShape.SetAsBox((bodyWidth / 2) / World::SCALE, (bodyHeight / 2) / World::SCALE);
	b2FixtureDef CubeFixtureDef;
	CubeFixtureDef.density = 1.5f;
	CubeFixtureDef.shape = &CubeShape;
	CubeFixtureDef.isSensor = true;
	//CubeFixtureDef.friction = 2.0f;
	CubeFixtureDef.filter.categoryBits = World::B2CollisionBits::SNAKE_BIT;
	mB2BodyFixture = mB2Body->CreateFixture(&CubeFixtureDef);

	mDebugCube.setOrigin(bodyWidth / 2, bodyHeight / 2);

	//Disable Collisions
	//mB2Body->GetFixtureList()->SetSensor(false);

}

void Snake::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
	if (World::DebugShowBounds) {
		target.draw(mDebugCube, states);
	}
}

void Snake::SetTarget(Hero* hero)
{
	mTargetHero = hero;
}

void Snake::updateCurrent(sf::Time dt) {

	centerOrigin(mSprite);

	float newX = mB2Body->GetPosition().x * World::SCALE;
	float newY = mB2Body->GetPosition().y * World::SCALE;
	setPosition(newX, newY);

	mAttackCooldownTimer += dt.asSeconds();

	if (mAttackCooldownTimer >= mAttackCooldown) {
		if (isTouchingPlayer) {
			mAttackCooldownTimer = 0;
			attackCurrentTarget();
		}
	}



	//ATTACK ANIMATION ------------------------------------------
	int width = 66;
	float timePerFrame = 1.0f;

	if (mAnimationWasTriggered) {
		int frameCount = 14;
		
		if (animClock.getElapsedTime().asSeconds() > .05f) {

			if (rectSourceSprite.left > width * frameCount) {
				rectSourceSprite.left = 0;
				mAnimationWasTriggered = false;
			}
			else {
				rectSourceSprite.left += width;
			}

			mSprite.setTextureRect(rectSourceSprite);
			animClock.restart();
		}
	}
	else {
		int frameCount = 4;

		if (animClock.getElapsedTime().asSeconds() > .1f) {

			if (rectSourceSprite.left > width * frameCount) {
				mIdleAnimIsReversed = true;
			}
			if (rectSourceSprite.left == 0) {
				mIdleAnimIsReversed = false;
			}

			if (!mIdleAnimIsReversed) {
				rectSourceSprite.left += width;
			}
			else {
				rectSourceSprite.left -= width;
			}

			mSprite.setTextureRect(rectSourceSprite);
			animClock.restart();
		}
	}


}

void Snake::attackCurrentTarget()
{
	mAnimationWasTriggered = true;
	mTargetHero->Damage(25);
}