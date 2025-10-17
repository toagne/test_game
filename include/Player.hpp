#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

class Player
{
private:
	sf::CircleShape player;

	bool isMoving;
	bool isAiming;
	bool inGoal;
	bool isResettingPos;
	float xAfterResetting;
	bool ballInGoal;

	sf::Vector2f delta;
	float len;
	sf::Vector2f step;

	float speed;
	float friction;
public:
	int index;
	Player();
	Player(float x, float y, int i, sf::Texture *ballTexture);
	~Player();

	void drawPlayer(sf::RenderTarget &target, bool firstTeamPlaying, bool playIsOn, bool someoneSelected, Player *selected);
	bool getClick(sf::Vector2f mouseLocalPosition);
	void aim(sf::Vector2f mouseLocalPosition, float speed = 0);
	bool checkIfAimingOrSelecting();
	void playerMoves();
	bool isInGoal(sf::FloatRect goal, std::string side = "none");
	bool checkCollisionWith(Player *b);
	void handleCollisionWith(Player *b);
	sf::Vector2f rotateVector(sf::Vector2f v, float angle);
	void drawAimingShapes(sf::RenderTarget &target, sf::Vector2f mousePos);
	void resetPos();

	void setIsMoving(bool value);
	const bool getIsMoving() const;
	const sf::Vector2f getPlayerCenterPos() const; 
	void changeStepDir(float x, float y);
	void setNewStep(sf::Vector2f new_step);
	void setNewPos(float x, float y);
	const float getR() const;
	const float getSpeed() const;
	void setNewSpeed(float new_speed);
	const sf::Vector2f getStep() const;
	sf::Color getColor();
	const int getIndex() const;
	const bool getIsAiming() const;
	void setInGoal(bool value);
	const bool getInGoal() const;
	void setIsResettingPos(bool value);
	const bool getIsResettingPos() const;
	void setXAfterResetting(float value);
	void setBallInGoal(bool value);
	const bool getBallInGoal() const;
};