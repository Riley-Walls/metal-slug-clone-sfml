#ifndef BOOK_RESOURCEIDENTIFIERS_HPP
#define BOOK_RESOURCEIDENTIFIERS_HPP


// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
}

namespace Textures
{

	enum ID
	{
		Eagle,
		Raptor,
		Desert,
		TitleScreen,
		RedBlock,
		BlueBlock,
		Forest,
		Sewer,
		Hero,
		Hero_Run_Legs,
		Hero_Upper_Body,
		Hero_Upper_Body_Grenade_Launcher,
		Hero_Upper_Body_Weapons,
		Crate,
		Snake,
		Snake_Anim,
		Coin,
		HealthPickup,
		Barrier,
		TallBarrier,
		Plank,
		Bullet,
		Missile,
		ButtonNormal,
		ButtonSelected,
		ButtonPressed,
		Tank,
		MissileTank,
		TankBullet,
	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}

// Forward declaration and a few type definitions
template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID>	TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID>			FontHolder;

#endif // BOOK_RESOURCEIDENTIFIERS_HPP
