#pragma once
#include <Book/Entity.hpp>
#include <Book/ResourceIdentifiers.hpp>

#include <SFML/Graphics/Sprite.hpp>

#include "Box2D\Box2D.h"
#include <SFML/Graphics/RectangleShape.hpp>

#include <Hero.hpp>

class InteractableObject
{

public:

	InteractableObject();
	virtual void Interact(Hero* hero);

private:


};

