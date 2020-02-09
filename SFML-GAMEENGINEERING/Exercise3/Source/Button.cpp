#include <Book/Button.hpp>
#include <Book/Utility.hpp>

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <iostream>
#include <string>
#include <Book/Application.hpp>

namespace GUI
{

Button::Button(const FontHolder& fonts, const TextureHolder& textures)
: mCallback()
, mNormalTexture(textures.get(Textures::ButtonNormal))
, mSelectedTexture(textures.get(Textures::ButtonSelected))
, mPressedTexture(textures.get(Textures::ButtonPressed))
, mSprite()
, mText("", fonts.get(Fonts::Main), 16)
, mIsToggle(false)
{
	mSprite.setTexture(mNormalTexture);

	sf::FloatRect bounds = mSprite.getLocalBounds();
	mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
}

void Button::setCallback(Callback callback)
{
	mCallback = std::move(callback);
}

void Button::setText(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}

void Button::setToggle(bool flag)
{
	mIsToggle = flag;
}

bool Button::isSelectable() const
{
    return true;
}

void Button::select()
{
	Component::select();

	mSprite.setTexture(mSelectedTexture);
}

void Button::deselect()
{
	Component::deselect();

	mSprite.setTexture(mNormalTexture);
}

void Button::activate()
{
	Component::activate();

    // If we are toggle then we should show that the button is pressed and thus "toggled".
	if (mIsToggle)
		mSprite.setTexture(mPressedTexture);

	if (mCallback)
		mCallback();

    // If we are not a toggle then deactivate the button since we are just momentarily activated.
	if (!mIsToggle)
		deactivate();
}

void Button::deactivate()
{
	Component::deactivate();

	if (mIsToggle)
	{
        // Reset texture to right one depending on if we are selected or not.
		if (isSelected())
			mSprite.setTexture(mSelectedTexture);
		else
			mSprite.setTexture(mNormalTexture);
	}
}

void Button::handleEvent(const sf::Event& event)
{
	//Get position of each corner
	sf::FloatRect bounds = mSprite.getLocalBounds();

	//std::cout << "Global Top: (" << "top: " << mSprite.getGlobalBounds().top << ", left: " << mSprite.getGlobalBounds().left << ") \n";

	//Handle mouse events
	if (event.type == sf::Event::MouseButtonPressed) {

		//Right-click
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {


			/*  //DEBUG
			std::cout << "Right-Click!\n";
			std::cout << "Mouse Position: (" << "x: " << Application::mouseX << ", y: " << Application::mouseY << ") \n";
			std::cout << "topLeftPosition.x: (" << "x: " << topLeftPosition.x << "\n";
			std::cout << "topRightPosition.x: (" << "x: " << topRightPosition.x << "\n";
			*/


			float leftPos = bounds.left + getPosition().x;
			float topPos = bounds.top + getPosition().y;

			if (Application::mouseX > leftPos && Application::mouseX < leftPos + bounds.width) {
				if (Application::mouseY > topPos && Application::mouseY < topPos + bounds.height) {
					std::cout << "HIT!\n";
					activate();
				}
			}

		}
	}
	
	/*
	//DEBUG MESSAGES
	std::string buttonName = mText.getString();
	std::cout << "Button Name: '" + buttonName + "\n";
	std::cout << "Top Left Position: 'x: " + std::to_string(topLeftPosition.x) + ", " + std::to_string(topLeftPosition.y) + "'\n";
	std::cout << "\n";
	*/

}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
	target.draw(mText, states);
}

}
