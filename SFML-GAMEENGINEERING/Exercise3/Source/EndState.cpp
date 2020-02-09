#include <Book/TitleState.hpp>
#include <Book/Utility.hpp>
#include <Book/ResourceHolder.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <EndState.hpp>

EndState::EndState(StateStack& stack, Context context)
: State(stack, context)
, mText()
, mShowText(true)
, mTextEffectTime(sf::Time::Zero)
{
	mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));
	mBackgroundSprite.setScale(1.6f, 1.35f);

	mText.setFont(context.fonts->get(Fonts::Main));
	mText.setString("We'll miss you. Have a nice day! Press any key to exit...");
	centerOrigin(mText);
	mText.setPosition(context.window->getView().getSize() / 2.f);
}

void EndState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	//window.draw(mBackgroundSprite);
	if (mShowText) {
		window.draw(mText);
	}

}

bool EndState::update(sf::Time dt)
{


	mTextEffectTime += dt;

	if (mTextEffectTime >= sf::seconds(0.5f))
	{
		mShowText = !mShowText;
		mTextEffectTime = sf::Time::Zero;
	}

	return true;
}

bool EndState::handleEvent(const sf::Event& event)
{
	// If any key is pressed, trigger the next screen
	
	if (event.type == sf::Event::KeyPressed)
	{
		requestStackPop();
	}
	

	return true;
}