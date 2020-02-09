#include <Book/World.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include <algorithm>
#include <cmath>

#include "Box2D\Box2D.h"
#include "Box2D\Common\b2Settings.h"
#include <iostream>

#include <B2SpriteNode.hpp>
#include <B2RectangleNode.hpp>
#include <Snake.hpp>
#include <HealthPickup.hpp>
#include <Coin.hpp>
#include <Bullet.hpp>
#include <Tank.hpp>


World::World(sf::RenderWindow& window)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mTextures() 
, mSceneGraph()
, mSceneLayers()
, mWorldBounds(0.f, 0.f, 8000.f, mWorldView.getSize().y)
, mSpawnPosition((mWorldView.getSize().x / 2.f) + 1800, mWorldBounds.height - mWorldView.getSize().y / 2.f)
, mScrollSpeed(-50.f)
, mPlayerAircraft(nullptr)
, gravity(0.0f, 25.0f)
, mB2World(gravity)
, mHero(nullptr)
, mFont()
, mHealthText()
, mCoinText()
, mElapsedTimeText()
, mB2WorldContactListener(new B2WorldContactListener)
{
	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
	//mWorldView.setViewport(sf::FloatRect(0, 0, 1, 1));
	
	//Box2D Collision Detection
	mB2World.SetContactListener(mB2WorldContactListener);
	
}

void World::update(sf::Time dt)
{

	//Update elapsed time
	mElapsedTime += dt.asSeconds();

	//Box2D physics step
	mB2World.Step(1 / 60.f, 8, 3);

	// Scroll the world, reset player velocity
	//BMRK_WINDOW_SCROLL
	//mWorldView.move(0 - (mScrollSpeed * dt.asSeconds()) , 0.f);

	float newViewX = mHero->mB2Body->GetPosition().x * World::SCALE;
	float newViewY = mWorldView.getCenter().y;
	mWorldView.setCenter(newViewX, newViewY);

	//mPlayerAircraft->setVelocity(0.f, 0.f);

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt);
	adaptPlayerPosition();

	//Update UI
	float newX = mWorldView.getCenter().x - (mWorldView.getSize().x / 2);
	float newY = mWorldView.getCenter().y - (mWorldView.getSize().y / 2);
	mUIBackground.setPosition(newX, newY);

	mHealthText.setPosition(newX + 10, newY + 20);
	mHealthText.setString("HEALTH: " + std::to_string(mHero->mHealth));

	mCoinText.setPosition(newX + 10, newY + 50);
	mCoinText.setString("COINS: " + std::to_string(mHero->mCoinCount));

	mElapsedTimeText.setPosition(newX + 10, newY + 80);
	mElapsedTimeText.setString("TIME: " + std::to_string((int)mElapsedTime));

}

void World::draw()
{
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);

	mWindow.draw(mUIBackground);
	mWindow.draw(mHealthText);
	mWindow.draw(mCoinText);
	mWindow.draw(mElapsedTimeText);

	//Box2d Test Code-------------------------------------------------------------------------------------------------------
	/*
	for (b2Body* BodyIterator = mB2World.GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext()) {
		if (BodyIterator->GetType() == b2_dynamicBody) {
			
			shape.setOrigin(50, 50);
			shape.setPosition(BodyIterator->GetPosition().x * 30, BodyIterator->GetPosition().y * 30);
			shape.setRotation(BodyIterator->GetAngle() * 180 / b2_pi);
			mWindow.draw(shape);
			
		}
		else if (BodyIterator->GetType() == b2_staticBody) {
			
			Gshape.setOrigin(400, 50);
			Gshape.setPosition(BodyIterator->GetPosition().x * 30, BodyIterator->GetPosition().y * 30);
			Gshape.setRotation(BodyIterator->GetAngle() * 180 / b2_pi);
			mWindow.draw(Gshape);
		}
	}
	*/
	//----END BOX2D-------------------------------------------------------------------------------------------------------
	
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::loadTextures()
{
	//BMRK_LOAD_TEXTURES
	mTextures.load(Textures::Eagle, "Media/Textures/Eagle.png");
	mTextures.load(Textures::Raptor, "Media/Textures/Raptor.png");
	mTextures.load(Textures::Desert, "Media/Textures/Desert.png");

	mTextures.load(Textures::RedBlock, "Media/Textures/red_cube.png");
	mTextures.load(Textures::BlueBlock, "Media/Textures/blue_cube.png");
	mTextures.load(Textures::Forest, "Media/Textures/forest.png");
	mTextures.load(Textures::Sewer, "Media/Textures/sewer.png");
	mTextures.load(Textures::Hero, "Media/Textures/hero_run.png");
	mTextures.load(Textures::Hero_Run_Legs, "Media/Textures/run_animsheet.png");
	mTextures.load(Textures::Hero_Upper_Body, "Media/Textures/hero_upper_body.png");
	mTextures.load(Textures::Hero_Upper_Body_Grenade_Launcher, "Media/Textures/hero_upper_body_grenade_launcher.png");
	mTextures.load(Textures::Hero_Upper_Body_Weapons, "Media/Textures/hero_upper_body_weapons.png");
	mTextures.load(Textures::Crate, "Media/Textures/crate.png");
	mTextures.load(Textures::Snake, "Media/Textures/snake.png");
	mTextures.load(Textures::Snake_Anim, "Media/Textures/snake_anim.png");
	mTextures.load(Textures::Coin, "Media/Textures/coin.png");
	mTextures.load(Textures::HealthPickup, "Media/Textures/health_pickup.png");
	mTextures.load(Textures::Barrier, "Media/Textures/barrier.png");
	mTextures.load(Textures::TallBarrier, "Media/Textures/tall_barrier.png");
	mTextures.load(Textures::Plank, "Media/Textures/plank.png");
	mTextures.load(Textures::Bullet, "Media/Textures/bullet.png");
	mTextures.load(Textures::Missile, "Media/Textures/missile.png");
	mTextures.load(Textures::Tank, "Media/Textures/tank.png");
	mTextures.load(Textures::MissileTank, "Media/Textures/missile_tank.png");
	mTextures.load(Textures::TankBullet, "Media/Textures/tank_bullet.png");



}

void World::buildScene()
{
	// Set up UI -------------------------------------------------------
	mUIBackground.setSize(sf::Vector2f(150, 120));
	mUIBackground.setFillColor(sf::Color::Black);

	float newX = mWorldView.getCenter().x;
	float newY = mWorldView.getCenter().y;
	//mUIBackground.setOrigin(mUIBackground.getSize().x / 2, mUIBackground.getSize().y / 2);
	mUIBackground.setPosition(newX, newY);

	mFont.loadFromFile("Media/Sansation.ttf");

	mHealthText.setPosition(100, 20);
	mHealthText.setCharacterSize(22);
	mHealthText.setFont(mFont);
	mHealthText.setFillColor(sf::Color::White);

	mCoinText.setPosition(100, 50);
	mCoinText.setCharacterSize(22);
	mCoinText.setFont(mFont);
	mCoinText.setFillColor(sf::Color::White);

	mElapsedTimeText.setPosition(100, 80);
	mElapsedTimeText.setCharacterSize(22);
	mElapsedTimeText.setFont(mFont);
	mElapsedTimeText.setFillColor(sf::Color::White);

	mHealthText.setString("HEALTH: " + std::to_string(50));
	mCoinText.setString("COINS: " + std::to_string(50));
	mElapsedTimeText.setString("TIME: " + std::to_string(50));


	// Initialize the different layers ---------------------------------------
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		SceneNode::Ptr layer(new SceneNode());
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& texture = mTextures.get(Textures::Sewer);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

	//BMRK_CREATE_HERO_IN_WORLD
	std::unique_ptr<Hero> hero(new Hero(mB2World, mTextures, mSceneLayers[Background]));
	mHero = hero.get();
	//mHero->setPosition(mSpawnPosition);
	mSceneLayers[Foreground]->attachChild(std::move(hero));

	//GROUND --------------------------------------------------------------------------------------
	std::unique_ptr<B2RectangleNode> groundB2RectangleNodePtr(new B2RectangleNode(mB2World, mTextures, 200, 600, 16000, 100, b2_staticBody));
	B2RectangleNode* groundB2SpriteNode = groundB2RectangleNodePtr.get();
	groundB2SpriteNode->SetColor(sf::Color::Transparent);
	mSceneLayers[Foreground]->attachChild(std::move(groundB2RectangleNodePtr));
	
	int groundLevel = 500;
	
	int spawnXPos = 0;

	//LEVEL OBJECTS --------------------------------------------------------------------------------------------------------
	
	B2SpriteNode* leftBarrier = createB2SpriteNode(600, groundLevel - 35, Textures::TallBarrier, b2_staticBody);
	leftBarrier->SetFriction(.0f);

	int crateHeight = 66;
	spawnXPos = 1200;

	

	//Bullet Tanks
	spawnXPos += 150;
	createTank(spawnXPos, mWorldBounds.height - mWorldView.getSize().y / 2.f, Tank::TankType::BULLET);
	spawnXPos += 200;
	createTank(spawnXPos, mWorldBounds.height - mWorldView.getSize().y / 2.f, Tank::TankType::BULLET);

	//Snake
	spawnXPos += 200;
	createSnake(spawnXPos);
	spawnXPos += 75;
	createSnake(spawnXPos);

	//Ramp / ------------------
	spawnXPos += 400;
	//Plank 
	B2SpriteNode* plank = createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 2), Textures::Plank, b2_dynamicBody);
	plank->SetFriction(0.2f);
	//Crates
	createB2SpriteNode(spawnXPos + 150, groundLevel - (crateHeight * 0), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 150, groundLevel - (crateHeight * 1), Textures::Crate, b2_dynamicBody);
	
	//Snake
	spawnXPos += 250;
	createSnake(spawnXPos);
	spawnXPos += 75;
	createSnake(spawnXPos);

	//Platform ---------------
	spawnXPos += 80;
	//Crates
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 0), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 1), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 2), Textures::Crate, b2_dynamicBody);
	//Plank
	B2SpriteNode* plank2 = createB2SpriteNode(spawnXPos + 200, groundLevel - (crateHeight * 2.5f), Textures::Plank, b2_dynamicBody);
	plank2->SetFriction(0.2f);
	//Crates
	createB2SpriteNode(spawnXPos + 400, groundLevel - (crateHeight * 0), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 400, groundLevel - (crateHeight * 1), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 400, groundLevel - (crateHeight * 2), Textures::Crate, b2_dynamicBody);

	//Missle Tanks
	spawnXPos += 150;
	createTank(spawnXPos, mWorldBounds.height - mWorldView.getSize().y / 2.f, Tank::TankType::MISSILE);
	spawnXPos += 200;
	createTank(spawnXPos, mWorldBounds.height - mWorldView.getSize().y / 2.f, Tank::TankType::MISSILE);


	//Small Pyramid------------
	spawnXPos += 600;
	createB2SpriteNode(spawnXPos, 500, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 25, 400, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 50, 500, Textures::Crate, b2_dynamicBody);

	//Snakes
	spawnXPos += 200;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);

	//2 Crate Stack w/ plank
	spawnXPos += 100;
	createB2SpriteNode(spawnXPos, 500, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 1), Textures::Crate, b2_dynamicBody);
	B2SpriteNode* plank3 = createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 2.2f), Textures::Plank, b2_dynamicBody);
	plank3->SetFriction(0.2f);

	//Coin
	createCoin(spawnXPos + 150, 175);

	//Snake
	spawnXPos += 100;
	createSnake(spawnXPos);

	//Coins
	spawnXPos += 400;
	createCoin(spawnXPos, 200);

	//Health
	spawnXPos += 50;
	createHealthPickup(spawnXPos, 450);


	//Small Pyramid
	spawnXPos += 100;
	createB2SpriteNode(spawnXPos, 500, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 25, 400, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 50, 500, Textures::Crate, b2_dynamicBody);

	//Missle Tanks
	spawnXPos += 150;
	createTank(spawnXPos, mWorldBounds.height - mWorldView.getSize().y / 2.f, Tank::TankType::MISSILE);
	spawnXPos += 200;
	createTank(spawnXPos, mWorldBounds.height - mWorldView.getSize().y / 2.f, Tank::TankType::MISSILE);

	//Tippy Crate Tower
	spawnXPos += 100;
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 0), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 1), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 2), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 3), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 40, groundLevel - (crateHeight * 4), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos - 40, groundLevel - (crateHeight * 4), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos, groundLevel - (crateHeight * 5), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 80, groundLevel - (crateHeight * 5), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos - 80, groundLevel - (crateHeight * 5), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 40, groundLevel - (crateHeight * 6), Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos - 40, groundLevel - (crateHeight * 6), Textures::Crate, b2_dynamicBody);

	//Snakes
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);

	//Small Pyramid
	spawnXPos += 200;
	createB2SpriteNode(spawnXPos, 500, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 25, 400, Textures::Crate, b2_dynamicBody);
	createB2SpriteNode(spawnXPos + 50, 500, Textures::Crate, b2_dynamicBody);
	createCoin(spawnXPos, 350);
	createCoin(spawnXPos + 25, 250);
	createCoin(spawnXPos + 50, 350);

	//Crate & Health
	spawnXPos += 450;
	createB2SpriteNode(spawnXPos, 500, Textures::Crate, b2_dynamicBody);
	createHealthPickup(spawnXPos, 450);

	//Snakes
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);
	spawnXPos += 100;
	createSnake(spawnXPos);
	spawnXPos += 100;
	createSnake(spawnXPos);
	createCoin(spawnXPos, 350);

	spawnXPos += 600;
	B2SpriteNode* rightBarrier = createB2SpriteNode(spawnXPos, groundLevel - 35, Textures::TallBarrier, b2_staticBody);
	rightBarrier->SetFriction(.0f);


	/*
	//HACK - Test bullet
	std::cout << "Creating Bullet\n";
	std::unique_ptr<Bullet> BulletPtr(new Bullet(mB2World, mTextures, 800, 200));
	Bullet* bullet = BulletPtr.get();
	mSceneLayers[Foreground]->attachChild(std::move(BulletPtr));
	std::cout << "Bullet Created\n";
	*/
	
}

B2SpriteNode* World::createB2SpriteNode(float posX, float posY, Textures::ID textureID, b2BodyType bodyType, bool setCollisionsEnabled, bool hasFixedRotation) {
	std::unique_ptr<B2SpriteNode> boxB2SpriteNodePtr(new B2SpriteNode(mB2World, posX, posY, mTextures, textureID, bodyType, hasFixedRotation));
	B2SpriteNode* boxB2SpriteNode = boxB2SpriteNodePtr.get();
	boxB2SpriteNode->SetCollisionsEnabled(setCollisionsEnabled);
	mSceneLayers[Foreground]->attachChild(std::move(boxB2SpriteNodePtr));
	return boxB2SpriteNode;
}

Tank* World::createTank(float posX, float posY, Tank::TankType tankType) {
	std::unique_ptr<Tank> TankPtr(new Tank(tankType, mB2World, mTextures, posX, posY, mHero, mSceneLayers[Background]));
	Tank* tank = TankPtr.get();
	mSceneLayers[Foreground]->attachChild(std::move(TankPtr));
	return tank;
}


Snake* World::createSnake(float posX) {

	std::unique_ptr<Snake> SnakePtr(new Snake(mB2World, mTextures, posX, 530.f));
	Snake* snake = SnakePtr.get();
	mSceneLayers[Foreground]->attachChild(std::move(SnakePtr));
	return snake;
	
}

Coin* World::createCoin(float posX, float posY) {
	std::unique_ptr<Coin> CoinPtr(new Coin(mB2World, posX, posY, mTextures, Textures::Coin, b2_staticBody, false));
	Coin* coin = CoinPtr.get();
	mSceneLayers[Foreground]->attachChild(std::move(CoinPtr));
	return coin;
}

HealthPickup* World::createHealthPickup(float posX, float posY) {
	std::unique_ptr<HealthPickup> HealthPickupPtr(new HealthPickup(mB2World, posX, posY, mTextures, Textures::HealthPickup, b2_dynamicBody, false));
	HealthPickup* healthPickup = HealthPickupPtr.get();
	mSceneLayers[Foreground]->attachChild(std::move(HealthPickupPtr));
	return healthPickup;
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	const float borderDistance = 40.f;

	/*
	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerAircraft->setPosition(position);
	*/
}

void World::adaptPlayerVelocity()
{
	/*
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));

	// Add scrolling velocity
	mPlayerAircraft->accelerate(0.f, mScrollSpeed);
	*/
}

bool World::CheckGameOver()
{
	if (mHero->mHealth <= 0) {
		return true;
	}
	else {
		return false;
	}
}