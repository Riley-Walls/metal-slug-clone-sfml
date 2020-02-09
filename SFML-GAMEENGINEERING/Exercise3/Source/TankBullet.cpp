#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <TankBullet.hpp>
#include <Book/World.hpp>

#include <iostream>


TankBullet::TankBullet(b2World& mB2World, const TextureHolder& textures, float x, float y)
	: mSprite(textures.get(Textures::TankBullet))
	, mB2Body(nullptr)
	, mVelocity(0, 0)
	, mB2World(mB2World)
{
	float bodyWidth = mSprite.getGlobalBounds().width;
	float bodyHeight = mSprite.getGlobalBounds().height;
	//int bodyWidth = 39;
	//int bodyHeight = 15;

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
	BodyDefCube.type = b2_dynamicBody;
	BodyDefCube.fixedRotation = true;
	BodyDefCube.userData = this;
	mB2Body = mB2World.CreateBody(&BodyDefCube);

	b2PolygonShape CubeShape;
	CubeShape.SetAsBox((bodyWidth / 2) / World::SCALE, (bodyHeight / 2) / World::SCALE);
	b2FixtureDef CubeFixtureDef;
	CubeFixtureDef.density = .0f;
	CubeFixtureDef.shape = &CubeShape;
	CubeFixtureDef.isSensor = false;
	CubeFixtureDef.friction = 2.0f;
	CubeFixtureDef.filter.categoryBits = World::B2CollisionBits::TANK_BULLET_BIT;
	mB2BodyFixture = mB2Body->CreateFixture(&CubeFixtureDef);

	mDebugCube.setOrigin(bodyWidth / 2, bodyHeight / 2);

	//Remove gravity
	//mB2Body->SetGravityScale(0);


	//Disable Collisions
	//mB2Body->GetFixtureList()->SetSensor(false);
	mSprite.scale(-1.f, 1);

}

void TankBullet::Shoot(bool isFacingRight) {
	if (isFacingRight) {
		mB2Body->ApplyLinearImpulse(b2Vec2(10, -10), mB2Body->GetWorldCenter(), true);
	}
	else {
		mB2Body->ApplyLinearImpulse(b2Vec2(-10, -10), mB2Body->GetWorldCenter(), true);
	}

}

void TankBullet::SetVelocity(int x, int y) {
	mVelocity.x = x;
	mVelocity.y = y;
}

void TankBullet::Destroy() {
	setToDestroy = true;
}

void TankBullet::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!destroyed) {
		target.draw(mSprite, states);
		if (World::DebugShowBounds) {
			target.draw(mDebugCube, states);
		}
	}
}

void TankBullet::SetTarget(Hero* hero)
{
	mTargetHero = hero;
}

void TankBullet::updateCurrent(sf::Time dt) {

	if (setToDestroy && !destroyed) {
		std::cout << "\nDestroy Body\n\n";
		mB2World.DestroyBody(mB2Body);
		destroyed = true;
	}
	else if (!setToDestroy && !destroyed){

		/*
		if (mB2Body->GetLinearVelocity().x <= 10.5 && mB2Body->GetLinearVelocity().x >= -10.5) {
			mB2Body->ApplyLinearImpulse(b2Vec2(mVelocity.x, mVelocity.y), mB2Body->GetWorldCenter(), true);
		}
		*/

		centerOrigin(mSprite);

		float newX = mB2Body->GetPosition().x * World::SCALE;
		float newY = mB2Body->GetPosition().y * World::SCALE;
		setPosition(newX, newY);

		//std::cout << "x: " + std::to_string(newX) + ", y: " + std::to_string(newY) + "\n";


		//Flips sprite based on velocity
		if (isFacingRight && mB2Body->GetLinearVelocity().x < -0.0001f) {
			isFacingRight = false;
			mSprite.scale(-1.f, 1);
		}
		if (!isFacingRight && mB2Body->GetLinearVelocity().x > 0.0001f) {
			isFacingRight = true;
			mSprite.scale(-1.f, 1);
		}

	}


}

void TankBullet::attackCurrentTarget()
{
	mAnimationWasTriggered = true;
	mTargetHero->Damage(25);
}