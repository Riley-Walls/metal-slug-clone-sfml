#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <B2RectangleNode.hpp>
#include <Book/World.hpp>

#include <iostream>


B2RectangleNode::B2RectangleNode(b2World& mB2World, const TextureHolder& textures, float x, float y, float w, float h, b2BodyType bodyType, bool hasFixedRotation)
	: mB2Body(nullptr)
{
	sf::Color defaultCubeColor(0, 255, 0, 100);

	mRectangleShape.setSize(sf::Vector2f(w, h));
	mRectangleShape.setFillColor(defaultCubeColor);

	mDebugRectangleShape.setSize(sf::Vector2f(w, h));
	mDebugRectangleShape.setFillColor(defaultCubeColor);


	b2BodyDef BodyDefCube;
	BodyDefCube.position = b2Vec2(x / World::SCALE, y / World::SCALE);
	BodyDefCube.type = bodyType;
	BodyDefCube.fixedRotation = hasFixedRotation;
	mB2Body = mB2World.CreateBody(&BodyDefCube);


	b2PolygonShape CubeShape;
	CubeShape.SetAsBox((w / 2) / World::SCALE, (h / 2) / World::SCALE);
	b2FixtureDef CubeFixtureDef;
	CubeFixtureDef.density = 1.f;
	CubeFixtureDef.shape = &CubeShape;
	CubeFixtureDef.friction = 2.f;
	mB2Body->CreateFixture(&CubeFixtureDef);


}

void B2RectangleNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mRectangleShape, states);
	if (World::DebugShowBounds) {
		target.draw(mDebugRectangleShape, states);
	}
}

unsigned int B2RectangleNode::getCategory() const
{
	return Category::None;
}

void B2RectangleNode::updateCurrent(sf::Time dt)
{
	mRectangleShape.setOrigin(mRectangleShape.getSize().x / 2, mRectangleShape.getSize().y / 2);
	mDebugRectangleShape.setOrigin(mDebugRectangleShape.getSize().x / 2, mDebugRectangleShape.getSize().y / 2);

	//Get Position & Rotation from B2Body
	float newX = mB2Body->GetPosition().x * World::SCALE;
	float newY = mB2Body->GetPosition().y * World::SCALE;
	setPosition(newX, newY);
	float newRot = (mB2Body->GetAngle() * 180 / b2_pi);
	setRotation(newRot);
}

void B2RectangleNode::SetColor(sf::Color color) {
	mRectangleShape.setFillColor(color);
}

void B2RectangleNode::SetFriction(float value) {
	mB2Body->GetFixtureList()->SetFriction(value);
}

//mB2BodyFixture->SetFriction(.0f);