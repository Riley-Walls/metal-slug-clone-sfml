#include <Book/Player.hpp>
#include <Book/CommandQueue.hpp>
#include <Book/Aircraft.hpp>
#include <Book/Foreach.hpp>

#include <map>
#include <string>
#include <algorithm>

#include <Hero.hpp>
#include <iostream>

struct AircraftMover
{
	AircraftMover(float vx, float vy)
		: velocity(vx, vy)
	{
	}

	void operator() (Aircraft& aircraft, sf::Time) const
	{
		aircraft.accelerate(velocity);
	}

	sf::Vector2f velocity;
};

struct HeroMover
{
	HeroMover(float vx, float vy)
		: velocity(vx, vy)
	{
	}

	void operator() (Hero& hero, sf::Time) const
	{
		if (hero.mB2Body->GetLinearVelocity().x <= 5.5 && hero.mB2Body->GetLinearVelocity().x >= -5.5)
			hero.mB2Body->ApplyLinearImpulse(b2Vec2(velocity.x, velocity.y), hero.mB2Body->GetWorldCenter(), true);
	}

	sf::Vector2f velocity;
};

struct HeroJump
{
	HeroJump()
	{
	}

	void operator() (Hero& hero, sf::Time) const
	{
		hero.Jump();
	}
};

struct HeroShooter
{
	HeroShooter()
	{
	}

	void operator() (Hero& hero, sf::Time) const
	{
		hero.Shoot();
	}

	sf::Vector2f velocity;
};

struct HeroWeaponChanger
{
	HeroWeaponChanger()
	{
	}

	void operator() (Hero& hero, sf::Time) const
	{
		hero.CycleWeapon();
	}

	sf::Vector2f velocity;
};


Player::Player()
{
	// Set initial key bindings
	mKeyBinding[sf::Keyboard::Left] = MoveLeft;
	mKeyBinding[sf::Keyboard::Right] = MoveRight;
	mKeyBinding[sf::Keyboard::Up] = Jump;
	mKeyBinding[sf::Keyboard::Space] = Shoot;
	mKeyBinding[sf::Keyboard::Tab] = ChangeWeapon;

	// Set initial action bindings
	initializeActions();

	// Assign all categories to player's aircraft
	FOREACH(auto & pair, mActionBinding)
		pair.second.category = Category::Hero;

	mActionBinding[Jump].category = Category::Hero;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
			commands.push(mActionBinding[found->second]);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	FOREACH(auto pair, mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
			commands.push(mActionBinding[pair.second]);
	}
}

void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	FOREACH(auto pair, mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

void Player::initializeActions()
{
	const float playerSpeed = 200.f;

	//mActionBinding[MoveLeft].action	 = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0.f));
	//mActionBinding[MoveRight].action = derivedAction<Aircraft>(AircraftMover(+playerSpeed, 0.f));
	//mActionBinding[MoveUp].action    = derivedAction<Aircraft>(AircraftMover(0.f, -playerSpeed));
	//mActionBinding[MoveDown].action  = derivedAction<Aircraft>(AircraftMover(0.f, +playerSpeed));

	mActionBinding[MoveLeft].action = derivedAction<Hero>(HeroMover(-13, 0));
	mActionBinding[MoveRight].action = derivedAction<Hero>(HeroMover(13, 0));
	mActionBinding[Jump].action = derivedAction<Hero>(HeroJump());
	mActionBinding[Shoot].action = derivedAction<Hero>(HeroShooter());
	mActionBinding[ChangeWeapon].action = derivedAction<Hero>(HeroWeaponChanger());

}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case Shoot:
	case Jump:
		return true;
	case ChangeWeapon:
	default:
		return false;
	}
}
