#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>

#include "utils.h"
#include "Grid.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "A lot of sand falling");

    std::srand(time(NULL));

    initializeConfig();

    sf::Clock clock;
    float dt{ 0.f };
    float inputAccumulator{ 0.f };
    float inputCooldown{ 0.f };
    float timeAccumulator{ 0.f };
    float FPSAccumulator{ 0.f };
    int FPSCounter{ 0 };
    float timeStep = 1.f / 60.f;
    sf::Vector2f mousePos;

    bool KeyF1Depressed{ false };
    bool KeyF1Pressed{ false };

    bool KeySDepressed{ false };
    bool KeySPressed{ false };

    bool KeyWDepressed{ false };
    bool KeyWPressed{ false };

    bool KeyRDepressed{ false };
    bool KeyRPressed{ false };

    sf::Font defaultFont;
    defaultFont.loadFromFile("Roboto-Bold.ttf");

    sf::Text helpText;
    helpText.setFont(defaultFont);
    helpText.setFillColor(sf::Color::White);
    helpText.setCharacterSize(12);
    helpText.setPosition(0, 0);

    Grid grid;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // time & miscellaneous
        mousePos = sf::Vector2f(sf::Mouse::getPosition(window));

        dt = clock.restart().asSeconds();
        timeAccumulator += dt;
        FPSAccumulator += dt;
        FPSCounter++;

        if (FPSAccumulator > 1.0f)
        {
            std::cout << "FPS: " << FPSCounter << "\n";
            FPSCounter = 0;
            FPSAccumulator -= 1.0f;
        }

        // collect input

        KeyF1Depressed = (KeyF1Pressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) ? true : false;
        KeyF1Pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F1) ? true : false;

        KeySDepressed = (KeySPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::S)) ? true : false;
        KeySPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S) ? true : false;

        KeyWDepressed = (KeyWDepressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::W)) ? true : false;
        KeyWPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::W) ? true : false;

        KeyRDepressed = (KeyRDepressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::R)) ? true : false;
        KeyRPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::R) ? true : false;

        // process input
        Globals::multiThreaded = KeyF1Depressed ? !Globals::multiThreaded : Globals::multiThreaded;


        // create particles
        if (
            ( sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right) ) &&
            (
                mousePos.x > 0 && mousePos.x < (Globals::gridSize - Globals::brushSize) * Globals::particleSize &&
                mousePos.y > 0 && mousePos.y < (Globals::gridSize - Globals::brushSize) * Globals::particleSize
                )
            )
        {
            PARTICLE_TYPE type;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if (KeySPressed)
                    type = PARTICLE_TYPE::SAND;
                else if (KeyWPressed)
                    type = PARTICLE_TYPE::WATER;
                else if (KeyRPressed)
                    type = PARTICLE_TYPE::ROCK;
                else
                    type = PARTICLE_TYPE::NOTHING;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                type = PARTICLE_TYPE::NOTHING;
            }
            for (int i = 0; i < Globals::brushSize; i++)
            {
                for (int j = 0; j < Globals::brushSize; j++)
                {
                    sf::Vector2f coords = convertWindowCoordinatesToWorldCoordinates(mousePos.x, mousePos.y);
                    grid.set(coords.x + i, coords.y + j, type);
                    //if (flipCoin())
                    //{
                    //    continue;
                    //}
                }
            }
        }

        // main loop 60 Hz
        if (timeAccumulator >= timeStep)
        {
            timeAccumulator -= timeStep;

            grid.update();

            // help text
            std::string helpString("F1 - toggle multithreading\n");
            helpString.append(Globals::multiThreaded ? "Multithreaded\n" : "Singlethreaded\n");
            helpText.setString(helpString);

            // drawing
            window.clear();
            window.draw(grid);
            window.draw(helpText);
            window.display();
        }
    }

    return 0;
}