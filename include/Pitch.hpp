#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

class Pitch
{
private:
	sf::Vector2f center;
	float pitchLength;
	float pitchWidth;
	float lineWidth;

	sf::RectangleShape pitch;
	sf::CircleShape midf;
	sf::RectangleShape vLine;

	sf::RectangleShape leftGoal;
	sf::RectangleShape rightGoal;

	// functions
	void initVariables(sf::Vector2u windowSize);
	void createPitch();
public:
	Pitch();
	Pitch(sf::Vector2u windowSize);
	~Pitch();

	void drawPitch(sf::RenderTarget &target);

	void checkBoundaries(Player *player);

	const sf::FloatRect getPitchBounds(std::string side = "none") const;

	void handleIfSomeoneIsInGoal(std::vector<Player> *players, int *blueScore, int *redScore, bool *gameOver);
};