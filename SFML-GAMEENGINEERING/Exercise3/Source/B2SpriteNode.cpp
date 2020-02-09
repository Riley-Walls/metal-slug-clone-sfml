#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <B2SpriteNode.hpp>
#include <Book/World.hpp>

#include <iostream>


B2SpriteNode::B2SpriteNode(b2World& mB2World, float xPos, float yPos, const TextureHolder& textures, Textures::ID textureID, b2BodyType bodyType, bool hasFixedRotation)
	: mB2Body(nullptr)
	, mSprite(textures.get(textureID))
	, mB2World(mB2World)
{
	centerOrigin(mSprite);

	float spriteWidth = mSprite.getGlobalBounds().width;
	float spriteHeight = mSprite.getGlobalBounds().height;

	//Debug Cube
	sf::Color cubeColor(0, 255, 0, 100);

	mDebugCube.setSize(sf::Vector2f(spriteWidth, spriteHeight));
	mDebugCube.setFillColor(cubeColor);

	//Physics Body
	b2BodyDef BodyDefCube;
	BodyDefCube.position = b2Vec2(xPos / World::SCALE, yPos / World::SCALE);
	BodyDefCube.type = bodyType;
	BodyDefCube.fixedRotation = hasFixedRotation;
	BodyDefCube.userData = this;
	mB2Body = mB2World.CreateBody(&BodyDefCube);

	b2PolygonShape CubeShape;
	CubeShape.SetAsBox((spriteWidth / 2) / World::SCALE, (spriteHeight / 2) / World::SCALE);
	b2FixtureDef CubeFixtureDef;
	CubeFixtureDef.density = 3.f;
	CubeFixtureDef.shape = &CubeShape;
	CubeFixtureDef.friction = 2.f;

	//HACK
	CubeFixtureDef.filter.categoryBits = World::B2CollisionBits::DEFAULT_BIT;
	mB2Body->CreateFixture(&CubeFixtureDef);


}

void B2SpriteNode::RedefineB2Body() {

	///mB2World.DestroyBody(mB2Body);
	///Redefine...
}

void B2SpriteNode::SetCollisionBits(uint16 categoryBits) {

	b2Fixture* fix = mB2Body->GetFixtureList();
	b2Filter filter = fix->GetFilterData();
	filter.categoryBits = categoryBits;
	fix->SetFilterData(filter);

	//mB2Body->GetFixtureList()->GetFilterData().SetC = collisionMask;
}

void B2SpriteNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isEnabled) {
		target.draw(mSprite, states);
		if (World::DebugShowBounds) {
			target.draw(mDebugCube, states);
		}
	}
}

unsigned int B2SpriteNode::getCategory() const
{
	return Category::None;
}

void B2SpriteNode::updateCurrent(sf::Time dt)
{
	if (mTimeToRedefineB2Body) {
		RedefineB2Body();
		mTimeToRedefineB2Body = false;
	}


	float spriteWidth = mSprite.getGlobalBounds().width;
	float spriteHeight = mSprite.getGlobalBounds().height;

	//Center Debug Cube
	mDebugCube.setOrigin(spriteWidth / 2, spriteHeight / 2);
	//Center Sprite
	centerOrigin(mSprite);

	//Get Position & Rotation from B2Body
	float newX = mB2Body->GetPosition().x * World::SCALE;
	float newY = mB2Body->GetPosition().y * World::SCALE;
	setPosition(newX, newY);
	float newRot = (mB2Body->GetAngle() * 180 / b2_pi);
	setRotation(newRot);


	// DISABLE / ENABLE
	if (isEnabled && mSetToDisabled) {
		isEnabled = false;
		mB2Body->SetActive(false);
	}
	if (!isEnabled && !mSetToDisabled) {
		isEnabled = true;
		mB2Body->SetActive(true);
	}

}

void B2SpriteNode::SetCollisionsEnabled(bool tf) {
	mB2Body->GetFixtureList()->SetSensor(!tf);
}

void B2SpriteNode::SetEnabled(bool setEnabled) {

	if (!isEnabled && setEnabled) {
		mSetToDisabled = false;
	}
	if (isEnabled && !setEnabled) {
		mSetToDisabled = true;
	}

	/*
	mSetToDisabled = true;
	isVisible = tf;
	SetCollisionsEnabled(tf);
	mB2Body->SetActive(false);
	*/
}

void B2SpriteNode::SetFriction(float value) {
	mB2Body->GetFixtureList()->SetFriction(value);
}