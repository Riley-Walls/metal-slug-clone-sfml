#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Grenade.hpp>
#include <Book/World.hpp>

#include <iostream>


Grenade::Grenade(b2World& mB2World, const TextureHolder& textures, float x, float y)
	: rectSourceSprite(0, 0, 62, 25)
	, mSprite(textures.get(Textures::Missile), rectSourceSprite)
	, mB2Body(nullptr)
	, mVelocity(0, 0)
	, mB2World(mB2World)
	, mCurrentTarget(nullptr)
{
	//float spriteWidth = mLegSprite.getGlobalBounds().width;
	//float spriteHeight = mLegSprite.getGlobalBounds().height;

	int bodyWidth = 39;
	int bodyHeight = 15;

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
	CubeFixtureDef.filter.categoryBits = World::B2CollisionBits::GRENADE_BIT;
	mB2BodyFixture = mB2Body->CreateFixture(&CubeFixtureDef);


	// CIRCLE SENSOR ------------------

	int radius = 200;

	sf::Color circleColor(0, 0, 255, 100);
	mDebugCircle.setRadius(radius);
	mDebugCircle.setFillColor(circleColor);

	mDebugCircle.setOrigin(radius, radius);

	b2CircleShape CircleShape;
	CircleShape.m_radius = radius / World::SCALE;
	b2FixtureDef CircleFixtureDef;
	CircleFixtureDef.shape = &CircleShape;
	CircleFixtureDef.filter.categoryBits = World::B2CollisionBits::GRENADE_SENSOR;
	CircleFixtureDef.isSensor = true;
	mB2CircleSensorFixture = mB2Body->CreateFixture(&CircleFixtureDef);

	// --------------------------------

	mDebugCube.setOrigin(bodyWidth / 5, bodyHeight / 5);

	//Remove gravity
	mB2Body->SetGravityScale(0);


	//Disable Collisions
	//mB2Body->GetFixtureList()->SetSensor(false);
	mSprite.scale(-1.f, 1);

}


void Grenade::guideTowards(sf::Vector2f position)
{
	mTargetDirection = unitVector(position - getWorldPosition());
	//std::cout << "\nworld x: " + std::to_string(getWorldPosition().x) + ", \n";
	//std::cout << "\nx: " + std::to_string(position.x) + ", \n";
}

void Grenade::SetVelocity(int x, int y) {
	mVelocity.x = x;
	mVelocity.y = y;
}

void Grenade::Destroy() {
	setToDestroy = true;
}

void Grenade::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!destroyed && !setToDestroy) {
		target.draw(mSprite, states);
		if (World::DebugShowBounds) {
			target.draw(mDebugCube, states);
			target.draw(mDebugCircle, states);
		}
	}

}

void Grenade::SetTarget(SceneNode* newTarget)
{
	std::cout << "\SetTarget()\n";

	if (mCurrentTarget == nullptr) {
		std::cout << "\SetTarget(): mCurrentTarget is nullptr\n";
		mCurrentTarget = newTarget;
	}
	else {
		std::cout << "\SetTarget(): mCurrentTarget exists\n";

	}
}

void Grenade::updateCurrent(sf::Time dt) {

	if (setToDestroy && !destroyed) {
		std::cout << "\nDestroy Body\n\n";
		mB2World.DestroyBody(mB2Body);
		destroyed = true;
	}
	else if (!setToDestroy && !destroyed){

		if (mCurrentTarget != nullptr) {
			guideTowards(mCurrentTarget->getPosition());

			const float approachRate = 200.f;

			sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * mTargetDirection + mVelocity);
			//newVelocity *= getMaxSpeed();
			float angle = std::atan2(newVelocity.y, newVelocity.x);
			mVelocity = newVelocity;

			//Fix
			if (isFacingRight) {
				angle -= 1.3f;
			}
			else {
				angle += 1.3f;
			}

			//std::cout << "\Angle: " + std::to_string(angle) + "\n";
			setRotation(toDegree(angle) + 90.f);
			mB2Body->ApplyForce(b2Vec2(mVelocity.x * 10, mVelocity.y * 10), mB2Body->GetWorldCenter(), true);
			//mB2Body->ApplyLinearImpulse(b2Vec2(mVelocity.x, mVelocity.y), mB2Body->GetWorldCenter(), true);

		}
		else {

			if (mB2Body->GetLinearVelocity().x <= 2.5 && mB2Body->GetLinearVelocity().x >= -2.5) {
				std::cout << "ApplyLinearImpulse";
				mB2Body->ApplyLinearImpulse(b2Vec2(mVelocity.x, mVelocity.y), mB2Body->GetWorldCenter(), true);
			}

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



		centerOrigin(mSprite);

		float newX = mB2Body->GetPosition().x * World::SCALE;
		float newY = mB2Body->GetPosition().y * World::SCALE;
		setPosition(newX, newY);


	}


}

void Grenade::attackCurrentTarget()
{
	mAnimationWasTriggered = true;
	mTargetHero->Damage(25);
}