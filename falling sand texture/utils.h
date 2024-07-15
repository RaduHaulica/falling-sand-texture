#pragma once

#include <SFML/Graphics.hpp>

struct Globals
{
	static const int gridStart = 0;
	static const int gridEnd = 1023;
	static const int gridSize = 512;
	static const int brushSize = 20;
	static const int particleSpeed = 10;
	static const int waterSpeed = 50;
	static const int particleSize = 2;

	static std::vector<sf::Color> particle_colors;

	static bool multiThreaded;
};

struct ParticleConfig
{
	sf::Color COLOR_NOTHING;
	sf::Color COLOR_SAND;
	sf::Color COLOR_WATER;
	sf::Color COLOR_ROCK;
};

enum PARTICLE_TYPE
{
	UNDEFINED = 0,
	NOTHING = 1,
	SAND = 2,
	WATER = 3,
	ROCK = 4
};

class Particle
{
public:
	sf::Color _color;
};

class ParticleFactory
{
public:
	static Particle createParticle(PARTICLE_TYPE type = PARTICLE_TYPE::SAND);
};

void initializeConfig();

int gridCoordinatesToGridPosition(int x, int y);
sf::Vector2f gridPositionToGridCoordinates(int position);

sf::Vector2f convertWindowCoordinatesToWorldCoordinates(int x, int y);
sf::Vector2f convertWorldCoordinatesToWindowCoordinates(int x, int y);

bool colorCompare(sf::Color color1, sf::Color color2);

PARTICLE_TYPE detectParticleType(sf::Uint8& particle);

bool flipCoin();

class Profiler
{
public:
	std::chrono::time_point<std::chrono::steady_clock> _startTime;
	std::string _label;

	Profiler(std::string label);
	~Profiler();
	double poke();
};