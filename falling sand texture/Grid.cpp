#include "Grid.h"

Grid::Grid()
{
	_grid = new sf::Uint8[Globals::gridSize * Globals::gridSize * 4];
	for (int i = 0; i < Globals::gridSize * Globals::gridSize * 4; i += 4)
	{
		Particle newParticle = ParticleFactory::createParticle(PARTICLE_TYPE::NOTHING);

		_grid[i] = newParticle._color.r;
		_grid[i + 1] = newParticle._color.g;
		_grid[i + 2] = newParticle._color.b;
		_grid[i + 3] = newParticle._color.a;
	}

	_gridTexture.create(Globals::gridSize * Globals::particleSize, Globals::gridSize * Globals::particleSize);
	_gridSprite.setTexture(_gridTexture);
}

Grid::~Grid()
{
	delete _grid;
}

void Grid::thread_render_function(int left, int right, int top, int bottom, sf::Uint8* pixelGrid)
{
	for (int i = left; i < right; i++)
	{
		for (int j = top; j < bottom; j++)
		{
			int currentPosition = gridCoordinatesToGridPosition(i, j);
			PARTICLE_TYPE baseType = detectParticleType(_grid[currentPosition]);
			Particle newParticle = ParticleFactory::createParticle(baseType);

			// for each particle create [ size x size ] pixels
			for (int k = 0; k < Globals::particleSize; k++)
			{
				for (int l = 0; l < Globals::particleSize; l++)
				{
					int position = j * Globals::gridSize * Globals::particleSize * 4 * Globals::particleSize + i * 4 * Globals::particleSize + l * Globals::gridSize * Globals::particleSize * 4 + k * 4;
					pixelGrid[position] = newParticle._color.r;
					pixelGrid[position + 1] = newParticle._color.g;
					pixelGrid[position + 2] = newParticle._color.b;
					pixelGrid[position + 3] = newParticle._color.a;
				}
			}
		}
	}
}


void Grid::render()
{
	sf::Uint8* _pixelGrid = new sf::Uint8[Globals::gridSize * Globals::gridSize * 4 * Globals::particleSize * Globals::particleSize];

	_threads.clear();

	for (int i = 0; i < 16; i++)
	{
		_threads.push_back(std::thread(
			&Grid::thread_render_function,
			this,
			i * (Globals::gridSize / 16),
			(i + 1) * (Globals::gridSize / 16),
			0,
			Globals::gridSize,
			_pixelGrid
		));
	}

	for (int i = 0; i < 16; i++)
	{
		_threads[i].join();
	}

	_threads.clear();

	//for (int i = 0; i < Globals::gridSize; i++)
	//{
	//	for (int j = 0; j < Globals::gridSize; j++)
	//	{
	//		int currentPosition = gridCoordinatesToGridPosition(i, j);
	//		PARTICLE_TYPE baseType = detectParticleType(_grid[currentPosition]);
	//		Particle newParticle = ParticleFactory::createParticle(baseType);

	//		// for each particle create [ size x size ] pixels
	//		for (int k = 0; k < Globals::particleSize; k++)
	//		{
	//			for (int l = 0; l < Globals::particleSize; l++)
	//			{
	//				int position = j * Globals::gridSize * Globals::particleSize * 4 * Globals::particleSize + i * 4 * Globals::particleSize + l * Globals::gridSize * Globals::particleSize * 4 + k * 4;
	//				_pixelGrid[position] = newParticle._color.r;
	//				_pixelGrid[position + 1] = newParticle._color.g;
	//				_pixelGrid[position + 2] = newParticle._color.b;
	//				_pixelGrid[position + 3] = newParticle._color.a;
	//			}
	//		}
	//	}
	//}

	_gridTexture.update(_pixelGrid);
	//_gridTexture.update(_grid);
	delete _pixelGrid;
}

void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(_gridSprite);
}

sf::Uint8* Grid::get(int x, int y)
{
	return &_grid[y * 4 * Globals::gridSize + x * 4];
}

void Grid::set(int x, int y, PARTICLE_TYPE type)
{
	//sf::Uint8* existingParticle = get(x, y);
	//if (detectParticleType(*existingParticle) != NOTHING)
	//	return;
	
	int position = y * Globals::gridSize * 4 + x * 4;
	Particle newParticle = ParticleFactory::createParticle(type);

	_grid[position] = newParticle._color.r;
	_grid[position + 1] = newParticle._color.g;
	_grid[position + 2] = newParticle._color.b;
	_grid[position + 3] = newParticle._color.a;
}

void Grid::update_particle(int x, int y)
{
	if (y >= Globals::gridSize - 1)
		return;

	int positionGrid = gridCoordinatesToGridPosition(x, y);
	PARTICLE_TYPE type = detectParticleType(_grid[positionGrid]);

	if (type == PARTICLE_TYPE::NOTHING)
		return;

	int steps = 0;
	int currentX = x;
	int currentY = y;

	switch (type)
	{
	case PARTICLE_TYPE::SAND:
	{
		while (steps < Globals::particleSpeed)
		{
			steps++;
			if ((currentX > 0) && (currentX < Globals::gridSize - 2) && (currentY <= Globals::gridSize - 2))
			{
				if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX, currentY + 1)]) == PARTICLE_TYPE::NOTHING || detectParticleType(_grid[gridCoordinatesToGridPosition(currentX, currentY + 1)]) == PARTICLE_TYPE::WATER)
				{
					set(currentX, currentY, detectParticleType(_grid[gridCoordinatesToGridPosition(currentX, currentY + 1)]));
					currentY++;
					set(currentX, currentY, PARTICLE_TYPE::SAND);

				}
				else if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX - 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING || detectParticleType(_grid[gridCoordinatesToGridPosition(currentX + 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING)
				{
					bool left{ false }, right{ false };
					if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX - 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING)
					{
						left = true;
					}
					if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX + 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING)
					{
						right = true;
					}
					set(currentX, currentY, PARTICLE_TYPE::NOTHING);
					int xOffset = -1 * (int)left + 1 * (int)right;
					if (left && right)
					{
						xOffset += (std::rand() % 2) * 2 - 1;
					}
					currentX += xOffset;
					currentY++;
					set(currentX, currentY, PARTICLE_TYPE::SAND);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		break;
	}
	case PARTICLE_TYPE::WATER:
	{
		bool directionDecided{ false };
		bool directionRight { false };
		while (steps < Globals::particleSpeed)
		{
			steps++;
			if ((currentX > 0) && (currentX < Globals::gridSize - 2) && (currentY <= Globals::gridSize - 2))
			{
				// down
				if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX, currentY + 1)]) == PARTICLE_TYPE::NOTHING )
				{
					set(currentX, currentY, detectParticleType(_grid[gridCoordinatesToGridPosition(currentX, currentY + 1)]));
					currentY++;
					set(currentX, currentY, PARTICLE_TYPE::WATER);

				}
				// down-left and down-right
				else if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX - 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING || detectParticleType(_grid[gridCoordinatesToGridPosition(currentX + 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING)
				{
					bool left{ false }, right{ false };
					if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX - 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING)
					{
						left = true;
					}
					if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX + 1, currentY + 1)]) == PARTICLE_TYPE::NOTHING)
					{
						right = true;
					}
					set(currentX, currentY, PARTICLE_TYPE::NOTHING);
					int xOffset = -1 * (int)left + 1 * (int)right;
					if (left && right)
					{
						xOffset += (std::rand() % 2) * 2 - 1;
					}
					currentX += xOffset;
					currentY++;
					set(currentX, currentY, PARTICLE_TYPE::WATER);
				}
				// left or right
				else if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX - 1, currentY)]) == PARTICLE_TYPE::NOTHING || detectParticleType(_grid[gridCoordinatesToGridPosition(currentX + 1, currentY)]) == PARTICLE_TYPE::NOTHING)
				{
					bool left{ false }, right{ false };
					int xOffset{ 0 };
					if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX - 1, currentY)]) == PARTICLE_TYPE::NOTHING)
					{
						left = true;
					}
					if (detectParticleType(_grid[gridCoordinatesToGridPosition(currentX + 1, currentY)]) == PARTICLE_TYPE::NOTHING)
					{
						right = true;
					}

					set(currentX, currentY, PARTICLE_TYPE::NOTHING);
					if (directionDecided)
					{
						if (directionRight && right)
							xOffset++;
						else if (!directionRight && left)
							xOffset--;
						steps -= (std::rand() % 4) / 2; // introduce some chaos in sideways water movement
					}
					else
					{
						xOffset = -1 * (int)left + 1 * (int)right;
						if (left && right)
						{
							directionDecided = true;
							directionRight = std::rand() % 2 == 1 ? true : false;
							xOffset += directionRight == true ? 1 : -1;
						}
					}
					currentX += xOffset;
					set(currentX, currentY, PARTICLE_TYPE::WATER);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		break;
	}
	}
}

void Grid::thread_update_function(int left, int right, int top, int bottom)
{
	for (int i = right; i >= left; i--)
	{
		for (int j = bottom; j >= top; j--)
		{
			update_particle(i, j);
		}
	}
}

void Grid::update_multithreaded()
{
	_threads.clear();

	for (int i = 0; i < 8; i++)
	{
		_threads.push_back(std::thread(
			&Grid::thread_update_function,
			this,
			i * (Globals::gridSize / 8),
			(i + 1) * (Globals::gridSize / 8) - 1,
			0,
			Globals::gridSize - 1
		));
	}

	for (int i = 0; i < 8; i++)
	{
		_threads[i].join();
	}

	_threads.clear();
}

void Grid::update_singlethreaded()
{
	for (int i = 0; i < Globals::gridSize; i++)
	{
		for (int j = 0; j < Globals::gridSize; j++)
		{
			update_particle(i, j);
		}
	}
}

void Grid::update()
{
	if (Globals::multiThreaded)
	{
		update_multithreaded();
	}
	else
	{
		update_singlethreaded();
	}

	render();
}