#include "utils.h"

std::vector<sf::Color> Globals::particle_colors = { sf::Color::Black, sf::Color::Yellow };

bool Globals::multiThreaded = true;

ParticleConfig CONFIG;

void initializeConfig()
{
	CONFIG.COLOR_NOTHING = sf::Color::Black;
	CONFIG.COLOR_SAND = sf::Color::Yellow;
	CONFIG.COLOR_WATER = sf::Color::Blue;
	CONFIG.COLOR_ROCK.r = 127;
	CONFIG.COLOR_ROCK.g = 127;
	CONFIG.COLOR_ROCK.b = 127;
	CONFIG.COLOR_ROCK.a = 255;
}

Particle ParticleFactory::createParticle(PARTICLE_TYPE type)
{
	Particle newParticle;
	switch (type)
	{
	case PARTICLE_TYPE::NOTHING:
	{
		newParticle._color = sf::Color::Black;
		break;
	}
	case PARTICLE_TYPE::SAND:
	{
		newParticle._color = sf::Color::Yellow;
		break;
	}
	case PARTICLE_TYPE::WATER:
	{
		newParticle._color = sf::Color::Blue;
		break;
	}
	case PARTICLE_TYPE::ROCK:
	{
		newParticle._color = CONFIG.COLOR_ROCK;
		break;
	}
	}

	return newParticle;
}

bool colorCompare(sf::Color color1, sf::Color color2)
{
	return ((std::abs(color1.r - color2.r) < 10) && (std::abs(color1.g - color2.g) < 10) && (std::abs(color1.b - color2.b) < 10));
}

int gridCoordinatesToGridPosition(int x, int y)
{
	return y * 4 * Globals::gridSize + x * 4;
}

sf::Vector2f gridPositionToGridCoordinates(int position)
{
	int y = position / Globals::gridSize;
	int x = (position - y * Globals::gridSize) / 4;
	return { float(x), float(y)};
}

sf::Vector2f convertWindowCoordinatesToWorldCoordinates(int x, int y)
{
	return { (float)(x / Globals::particleSize), (float)(y / Globals::particleSize) };
}

sf::Vector2f convertWorldCoordinatesToWindowCoordinates(int x, int y)
{
	return { (float)(x * Globals::particleSize), (float)(y * Globals::particleSize) };
}

PARTICLE_TYPE detectParticleType(sf::Uint8& particle)
{
	sf::Color testColor;
	testColor.r = particle;
	testColor.g = *(&particle + 1);
	testColor.b = *(&particle + 2);
	testColor.a = *(&particle + 3);

	if (colorCompare(testColor, CONFIG.COLOR_SAND))
	{
		return PARTICLE_TYPE::SAND;
	}

	if (colorCompare(testColor, CONFIG.COLOR_WATER))
	{
		return PARTICLE_TYPE::WATER;
	}

	if (colorCompare(testColor, CONFIG.COLOR_ROCK))
	{
		return PARTICLE_TYPE::ROCK;
	}

	return PARTICLE_TYPE::NOTHING;
}

bool flipCoin()
{
	return std::rand() % 2;
}

Profiler::Profiler(std::string label) :
		_label{ label }
{
	_startTime = std::chrono::steady_clock::now();
	std::printf(std::format("{} - started.\n", _label).c_str());
};

Profiler::~Profiler()
{
	std::printf(std::format("{} - time passed: {} seconds.\n", _label, std::chrono::duration<double>(std::chrono::steady_clock::now() - _startTime).count()).c_str());
}

double Profiler::poke()
{
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - _startTime).count();
}