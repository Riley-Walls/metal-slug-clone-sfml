#include <Book/TitleState.hpp>
#include <Book/Utility.hpp>
#include <Book/ResourceHolder.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <GameOverState.hpp>

GameOverState::GameOverState(StateStack& stack, Context context)
: State(stack, context)
, mText()
, mShowText(true)
, mTextEffectTime(sf::Time::Zero)
{
	mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));
	mBackgroundSprite.setScale(1.6f, 1.35f);

	mText.setFont(context.fonts->get(Fonts::Main));
	mText.setString("Game Over!!");
	centerOrigin(mText);
	mText.setPosition(context.window->getView().getSize() / 2.f);
}

void GameOverState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	//window.draw(mBackgroundSprite);
	window.draw(mText);
}

bool GameOverState::update(sf::Time dt)
{
	mStateExpireTime += dt;
	if (mStateExpireTime >= sf::seconds(5.f)) {
		requestStackPop();
		requestStackPush(States::Menu);
	}

	mTextEffectTime += dt;

	if (mTextEffectTime >= sf::seconds(0.5f))
	{
		mShowText = !mShowText;
		mTextEffectTime = sf::Time::Zero;
	}

	return true;
}

bool GameOverState::handleEvent(const sf::Event& event)
{
	// If any key is pressed, trigger the next screen
	/*
	if (event.type == sf::Event::KeyPressed)
	{
		requestStackPop();
		requestStackPush(States::Menu);
	}
	*/

	return true;
}