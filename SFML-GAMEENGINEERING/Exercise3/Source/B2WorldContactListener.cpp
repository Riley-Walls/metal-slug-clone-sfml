#include <Book/ResourceHolder.hpp>
#include <Book/Utility.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Book/World.hpp>
#include <iostream>

#include <InteractableObject.hpp>
#include <B2WorldContactListener.hpp>
#include <Snake.hpp>
#include <HealthPickup.hpp>
#include <Coin.hpp>
#include <Hero.hpp>
#include <Bullet.hpp>
#include <TankBullet.hpp>
#include <Grenade.hpp>
#include "Box2D\Box2D.h"

B2WorldContactListener::B2WorldContactListener()
{

}

void B2WorldContactListener::BeginContact(b2Contact* contact) {
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();

	uint16 cDef = fixA->GetFilterData().categoryBits | fixB->GetFilterData().categoryBits;

	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::HERO_BIT && fixB->IsSensor() == false) {
		Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
		hero->mCollisionCount++;
	} else if (fixB->GetFilterData().categoryBits == World::B2CollisionBits::HERO_BIT && fixA->IsSensor() == false) {
		Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
		hero->mCollisionCount++;
	}
	
	//BULLET COLLIDES WITH ANYTHING
	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::BULLET_BIT && fixB->IsSensor() == false) {
		Bullet* bullet = (Bullet*)fixA->GetBody()->GetUserData();
		bullet->Destroy();
	}
	else if (fixB->GetFilterData().categoryBits == World::B2CollisionBits::BULLET_BIT && fixA->IsSensor() == false) {
		Bullet* bullet = (Bullet*)fixB->GetBody()->GetUserData();
		bullet->Destroy();
	}

	//TANK BULLET COLLIDES WITH ANYTHING
	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::TANK_BULLET_BIT && fixB->IsSensor() == false) {
		TankBullet* bullet = (TankBullet*)fixA->GetBody()->GetUserData();
		bullet->Destroy();
	}
	else if (fixB->GetFilterData().categoryBits == World::B2CollisionBits::TANK_BULLET_BIT && fixA->IsSensor() == false) {
		TankBullet* bullet = (TankBullet*)fixB->GetBody()->GetUserData();
		bullet->Destroy();
	}

	//GRENADE COLLIDES WITH ANYTHING
	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_BIT && fixB->IsSensor() == false) {
		Grenade* grenade = (Grenade*)fixA->GetBody()->GetUserData();
		grenade->Destroy();
	}
	else if (fixB->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_BIT && fixA->IsSensor() == false) {
		Grenade* grenade = (Grenade*)fixB->GetBody()->GetUserData();
		grenade->Destroy();
	}

	/*
	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_SENSOR && fixB->IsSensor() == false) {
		Grenade* grenade = (Grenade*)fixA->GetBody()->GetUserData();
		//grenade->Destroy();
	}
	else if (fixB->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_SENSOR && fixA->IsSensor() == false) {
		Grenade* grenade = (Grenade*)fixB->GetBody()->GetUserData();
		//grenade->Destroy();
	}
	*/

	
	switch (cDef) {
	case World::B2CollisionBits::GRENADE_BIT | World::B2CollisionBits::SNAKE_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_SENSOR) {

			Grenade* grenade = (Grenade*)fixA->GetBody()->GetUserData();
			grenade->Destroy();
		}
		else {

			Grenade* grenade = (Grenade*)fixB->GetBody()->GetUserData();
			grenade->Destroy();
		}
		break;
	
		
	// Damage to tanks
	case World::B2CollisionBits::TANK_BIT | World::B2CollisionBits::GRENADE_BIT:
	case World::B2CollisionBits::TANK_BIT | World::B2CollisionBits::BULLET_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::TANK_BIT) {
			Tank* tank = (Tank*)fixA->GetBody()->GetUserData();
			tank->Destroy();
		}
		else {
			Tank* tank = (Tank*)fixB->GetBody()->GetUserData();
			tank->Destroy();
		}
		break;
	// Damage to hero
	case World::B2CollisionBits::HERO_BIT | World::B2CollisionBits::GRENADE_BIT:
	case World::B2CollisionBits::HERO_BIT | World::B2CollisionBits::TANK_BULLET_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_BIT || fixA->GetFilterData().categoryBits == World::B2CollisionBits::TANK_BULLET_BIT) {
			Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
			hero->Damage(25);
		}
		else {
			Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
			hero->Damage(25);
		}
		break;
	// MISSILE SENSOR ---------------------------------------------------------------------------------------
	case World::B2CollisionBits::GRENADE_SENSOR | World::B2CollisionBits::SNAKE_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_SENSOR) {
			Grenade* grenade = (Grenade*)fixA->GetBody()->GetUserData();
			if (grenade->mGrenadeType == Grenade::GrenadeType::HERO) {
				std::cout << "grenade->mGrenadeType == Grenade::GrenadeType::HERO\n";
				grenade->SetTarget((SceneNode*)fixB->GetBody()->GetUserData());
			}
		}
		else {
			Grenade* grenade = (Grenade*)fixB->GetBody()->GetUserData();
			if (grenade->mGrenadeType == Grenade::GrenadeType::HERO) {
				std::cout << "grenade->mGrenadeType == Grenade::GrenadeType::HERO\n";

				grenade->SetTarget((SceneNode*)fixA->GetBody()->GetUserData());
			}
		}
		break;
	case World::B2CollisionBits::GRENADE_SENSOR | World::B2CollisionBits::TANK_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_SENSOR) {
			Grenade* grenade = (Grenade*)fixA->GetBody()->GetUserData();
			if (grenade->mGrenadeType == Grenade::GrenadeType::HERO) {
				std::cout << "grenade->mGrenadeType == Grenade::GrenadeType::HERO\n";
				grenade->SetTarget((SceneNode*)fixB->GetBody()->GetUserData());
			}
		}
		else {
			Grenade* grenade = (Grenade*)fixB->GetBody()->GetUserData();
			if (grenade->mGrenadeType == Grenade::GrenadeType::HERO) {
				std::cout << "grenade->mGrenadeType == Grenade::GrenadeType::HERO\n";

				grenade->SetTarget((SceneNode*)fixA->GetBody()->GetUserData());
			}
		}
		break;
	case World::B2CollisionBits::GRENADE_SENSOR | World::B2CollisionBits::HERO_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::GRENADE_SENSOR) {
			Grenade* grenade = (Grenade*)fixA->GetBody()->GetUserData();
			if (grenade->mGrenadeType == Grenade::GrenadeType::ENEMY) {
				std::cout << "grenade->mGrenadeType == Grenade::GrenadeType::HERO\n";
				grenade->SetTarget((SceneNode*)fixB->GetBody()->GetUserData());
			}
		}
		else {
			Grenade* grenade = (Grenade*)fixB->GetBody()->GetUserData();
			if (grenade->mGrenadeType == Grenade::GrenadeType::ENEMY) {
				std::cout << "grenade->mGrenadeType == Grenade::GrenadeType::HERO\n";

				grenade->SetTarget((SceneNode*)fixA->GetBody()->GetUserData());
			}
		}
		break;
	// --------------------------------------------------------------------------------------------------------
	case World::B2CollisionBits::HERO_BIT | World::B2CollisionBits::SNAKE_BIT :

		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::SNAKE_BIT) {

			Snake* snake = (Snake*)fixA->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
			snake->isTouchingPlayer = true;
			snake->SetTarget(hero);
		}
		else {

			Snake* snake = (Snake*)fixB->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
			snake->isTouchingPlayer = true;
			snake->SetTarget(hero);
		}
		break;
	case World::B2CollisionBits::HERO_BIT | World::B2CollisionBits::HEALTH_PICKUP_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::HEALTH_PICKUP_BIT) {

			HealthPickup* healthPickup = (HealthPickup*)fixA->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
			healthPickup->SetTarget(hero);
			healthPickup->PickUp();
		}
		else {
			HealthPickup* healthPickup = (HealthPickup*)fixB->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
			healthPickup->SetTarget(hero);
			healthPickup->PickUp();
		}
		break;
	case World::B2CollisionBits::HERO_BIT | World::B2CollisionBits::COIN_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::COIN_BIT) {

			Coin* coin = (Coin*)fixA->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
			coin->SetTarget(hero);
			coin->PickUp();

		}
		else {

			Coin* coin = (Coin*)fixB->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
			coin->SetTarget(hero);
			coin->PickUp();
		}
		break;
	default:

		break;
	}
	
}
void B2WorldContactListener::EndContact(b2Contact* contact) {

	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();

	int cDef = fixA->GetFilterData().categoryBits | fixB->GetFilterData().categoryBits;

	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::HERO_BIT && fixB->IsSensor() == false) {
		Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
		hero->mCollisionCount--;
	}
	else if (fixB->GetFilterData().categoryBits == World::B2CollisionBits::HERO_BIT && fixA->IsSensor() == false) {
		Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
		hero->mCollisionCount--;
	}


	switch (cDef) {
	case World::B2CollisionBits::HERO_BIT | World::B2CollisionBits::SNAKE_BIT:
		if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::SNAKE_BIT) {
			Snake* snake = (Snake*)fixA->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixB->GetBody()->GetUserData();
			snake->isTouchingPlayer = false;
		}
		else {
			Snake* snake = (Snake*)fixB->GetBody()->GetUserData();
			Hero* hero = (Hero*)fixA->GetBody()->GetUserData();
			snake->isTouchingPlayer = false;
		}

		break;
	default:

		break;
	}
}
void B2WorldContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
	/*
	b2Fixture* fixA = contact->GetFixtureA();
	b2Fixture* fixB = contact->GetFixtureB();

	b2Fixture* heroFixture(nullptr);

	uint16 cDef = fixA->GetFilterData().categoryBits | fixB->GetFilterData().categoryBits;

	if (fixA->GetFilterData().categoryBits == World::B2CollisionBits::HERO_BIT) {
		heroFixture = fixA;
	}
	else {
		heroFixture = fixB;
	}
	*/
}
void B2WorldContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

}

void B2WorldContactListener::foo()
{

}