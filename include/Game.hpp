#pragma once

#include <SFML/Graphics.hpp>

class Game
{
private:
	sf::RenderWindow *window;

	void initVariables();
	void initWindow();
public:
	Game();
	~Game();

	const bool getWindowIsOpen() const;

	void gameUpdate();
	void gameRender();
	void pollEvents();
};
