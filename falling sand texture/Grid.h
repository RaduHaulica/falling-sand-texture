#pragma once

#include <SFML/Graphics.hpp>

#include <thread>
#include <iostream>

#include "utils.h"

class Grid : public sf::Drawable
{
public:
	sf::Uint8* _grid;
	sf::Texture _gridTexture;
	sf::Sprite _gridSprite;

	std::vector<std::thread> _threads;

	Grid();
	~Grid();
	void render();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::Uint8* get(int x, int y);
	void set(int x, int y, PARTICLE_TYPE type = PARTICLE_TYPE::SAND);

	void update_particle(int x, int y);
	void thread_update_function(int left, int top, int right, int bottom);
	void thread_render_function(int left, int top, int right, int bottom);
	void update_multithreaded();
	void update_singlethreaded();
	void update();
};