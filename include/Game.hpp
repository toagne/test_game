#pragma once

#include "Pitch.hpp"
#include <sstream>

class Pitch;

class Game
{
private:
	sf::RenderWindow window;
	sf::VideoMode videoMode;

	Pitch pitch;

	Player *selected;

	std::vector<Player> players;

	bool playIsOn;
	bool firstTeamPlaying;
	int redScore;
	int blueScore;
	bool gameOver;

	sf::Font font;
	sf::Text scoreText;
	sf::Text winnerText;

	// void initVariables();
	void initWindow();

	sf::Vector2f getMousePos();

	void initPlayers(unsigned int w, unsigned int h);
	std::vector<sf::Vector2f> getStartingPositions(unsigned int w, unsigned int h);
	Player* selectPlayer(sf::Vector2f mouseLocalPosition, bool firstTeamPlaying);
	
	void pollEvents();

	void drawTeam();
	void drawTexts();
public:
	Game();
	~Game();

	void reInitPlayersPos();

	const bool getWindowIsOpen() const;

	void gameUpdate();
	void gameRender();
};
