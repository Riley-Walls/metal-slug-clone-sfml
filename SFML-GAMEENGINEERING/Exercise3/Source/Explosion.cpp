#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Explosion.hpp>
#include <Book/World.hpp>

#include <iostream>


Explosion::Explosion(b2World& mB2World, const TextureHolder& textures, float x, float y)
	: rectSourceSprite(0, 0, 212, 234)
	, mSprite(textures.get(Textures::Explosion), rectSourceSprite)
{
	setPosition(x, y);
}

void Explosion::Explode() {
	mAnimationWasTriggered = true;
}

void Explosion::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
	if (World::DebugShowBounds) {
		target.draw(mDebugCube, states);
	}
}

void Explosion::updateCurrent(sf::Time dt) {

	centerOrigin(mSprite);

	//ATTACK ANIMATION ------------------------------------------
	int width = 212;
	float timePerFrame = 1.0f;

	if (mAnimationWasTriggered) {
		int frameCount = 23;
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


}
