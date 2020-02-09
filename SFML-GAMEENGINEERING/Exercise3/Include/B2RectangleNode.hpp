#pragma once
#include <Book/Entity.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>

#include "Box2D\Box2D.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <Book/ResourceIdentifiers.hpp>

class B2RectangleNode : public Entity
{

public:
	B2RectangleNode(b2World& mB2World, const TextureHolder& textures, float x, float y, float w, float h, b2BodyType bodyType, bool hasFixedRotation = false);
	virtual unsigned int	getCategory() const;
	virtual void		updateCurrent(sf::Time dt);

	b2Body* mB2Body;

	void					SetColor(sf::Color color);
	void					SetFriction(float value);
private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


private:
	sf::RectangleShape		mRectangleShape;
	sf::RectangleShape		mDebugRectangleShape;
};

