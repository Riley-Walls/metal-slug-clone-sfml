#ifndef BOOK_WORLD_HPP
#define BOOK_WORLD_HPP

#include <Book/ResourceHolder.hpp>
#include <Book/ResourceIdentifiers.hpp>
#include <Book/SceneNode.hpp>
#include <Book/SpriteNode.hpp>
#include <Book/Aircraft.hpp>
#include <Book/CommandQueue.hpp>
#include <Book/Command.hpp>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>

#include "Box2D\Box2D.h"
#include <SFML/Graphics/RectangleShape.hpp>

#include <Hero.hpp>
#include <B2SpriteNode.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <B2WorldContactListener.hpp>
#include <Snake.hpp>
#include <Tank.hpp>
#include <HealthPickup.hpp>
#include <Coin.hpp>

// Forward declaration
namespace sf
{
	class RenderWindow;
}

class World : private sf::NonCopyable
{
	public:
		explicit							World(sf::RenderWindow& window);
		void								update(sf::Time dt);
		void								draw();
		
		CommandQueue&						getCommandQueue();

		bool								CheckGameOver();

		//STATIC
		static constexpr double SCALE = 30.0;
		static constexpr bool DebugShowBounds = false;

		//BOX2D COLLISIONS ----------------------------
		enum B2CollisionBits
		{
			NONE_BIT			= 0,
			DEFAULT_BIT			= 1 << 0,
			HERO_BIT			= 1 << 1,
			GROUND_BIT			= 1 << 2,
			SNAKE_BIT			= 1 << 3,
			COIN_BIT			= 1 << 4,
			HEALTH_PICKUP_BIT	= 1 << 5,
			BULLET_BIT			= 1 << 6,
			GRENADE_BIT			= 1 << 7,
			GRENADE_SENSOR		= 1 << 8,
			TANK_BULLET_BIT		= 1 << 9,
			TANK_BIT			= 1 << 10,
		};
		//--------------------------------------------

	private:
		void								loadTextures();
		void								buildScene();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();

		float		mElapsedTime;

		//BOX2D COLLISIONS
		B2WorldContactListener*				mB2WorldContactListener;
		
	private:
		enum Layer
		{
			Background,
			Foreground,
			LayerCount
		};


	private:
		sf::RenderWindow&					mWindow;
		sf::View							mWorldView;
		TextureHolder						mTextures;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		float								mScrollSpeed;
		Aircraft*							mPlayerAircraft;

		//BOX2D ----------------------
		sf::RectangleShape shape;
		sf::RectangleShape Gshape;
		b2Vec2 gravity;
		b2World mB2World;
		b2BodyDef groundBodyDef;
		b2Body* groundBody;
		b2PolygonShape groundBox;
		b2BodyDef bodyDef;
		b2Body* body;
		b2PolygonShape dynamicBox;
		b2FixtureDef fixtureDef;
		float32 timeStep = 1.0f / 60.0f;
		int32 velocityIterations = 6;
		int32 positionIterations = 2;

		B2SpriteNode* createB2SpriteNode(float posX, float posY, Textures::ID textureID, b2BodyType bodyType, bool setCollisionsEnabled = true, bool hasFixedRotation = false);

		//Other ------------------------
		Hero* mHero;

		//UI --------------------------
		sf::RectangleShape		mUIBackground;
		sf::Text mHealthText;
		sf::Text mCoinText;
		sf::Text mElapsedTimeText;
		sf::Font	mFont;

		//Scene Creation
		Snake* createSnake(float posX);
		Tank* createTank(float posX, float posY, Tank::TankType tankType);
		Coin* createCoin(float posX, float posY);
		HealthPickup* createHealthPickup(float posX, float posY);
};

#endif // BOOK_WORLD_HPP
