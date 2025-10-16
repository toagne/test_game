#include "Pitch.hpp"

Pitch::Pitch()
{
}

Pitch::Pitch(sf::Vector2u windowSize)
{
	this->initVariables(windowSize);
	this->createPitch();

}

Pitch::~Pitch()
{
}

void Pitch::initVariables(sf::Vector2u windowSize)
{
	this->center = {static_cast<float>(windowSize.x / 2), static_cast<float>(windowSize.y / 2)};
	this->pitchLength = static_cast<int>(windowSize.x / 1000) * 1000.f;
	this->pitchWidth = this->pitchLength / 2.f;
	this->lineWidth = 5.f;
}

void Pitch::createPitch()
{
	// outeside lines
	this->pitch.setSize({this->pitchLength, this->pitchWidth});
	this->pitch.setPosition({(this->center.x - this->pitchLength / 2.f), (this->center.y - this->pitchWidth / 2.f)});
	this->pitch.setFillColor(sf::Color::Green);
	this->pitch.setOutlineThickness(this->lineWidth);
	this->pitch.setOutlineColor(sf::Color::White);

	//midfiled
	this->midf.setRadius(this->pitchWidth / 4);
	this->midf.setPosition(sf::Vector2f(center.x - midf.getRadius(), center.y - midf.getRadius()));
	this->midf.setFillColor(sf::Color::Green);
	this->midf.setOutlineColor(sf::Color::White);
	this->midf.setOutlineThickness(this->lineWidth);
	this->vLine.setSize({this->lineWidth, this->pitchWidth});
	this->vLine.setPosition({center.x, (this->center.y - this->pitchWidth / 2.f)});

	// draw goals
	float h_goal = (static_cast<int>(this->pitchWidth / 3 / 10) * 10); // player diam ~= 1/4 * h_goal
	this->leftGoal.setSize({h_goal / 2, h_goal});
	this->leftGoal.setPosition({
		this->pitch.getGlobalBounds().position.x - this->leftGoal.getGlobalBounds().size.x,
		this->pitch.getGlobalBounds().position.y + this->leftGoal.getGlobalBounds().size.y
	});
	this->leftGoal.setFillColor(sf::Color::Green);
	this->leftGoal.setOutlineThickness(this->lineWidth);
	this->leftGoal.setOutlineColor(sf::Color::White);
	this->rightGoal = this->leftGoal;
	this->rightGoal.move({this->pitch.getGlobalBounds().size.x + this->leftGoal.getGlobalBounds().size.x - this->lineWidth * 2, 0.f});
}

void Pitch::drawPitch(sf::RenderTarget &target)
{
	target.draw(this->pitch);
	target.draw(this->midf);
	target.draw(this->vLine);
	target.draw(this->leftGoal);
	target.draw(this->rightGoal);
}

void Pitch::checkBoundaries(Player *player)
{
	// left
	if (player->getPlayerCenterPos().x - player->getR() + (player->getStep().x * player->getSpeed()) < this->pitch.getGlobalBounds().position.x) {
		if (!player->isInGoal(this->leftGoal.getGlobalBounds())) {
			player->setNewPos(
				this->pitch.getGlobalBounds().position.x,
				player->getPlayerCenterPos().y - player->getR()
			);
			player->changeStepDir(-1.f, 1.f);
			player->setNewSpeed(player->getSpeed() * 0.9f);
		}
		// collision inside left goal
		else {
			if (player->getColor() == sf::Color::White)
				if (player->getPlayerCenterPos().x + player->getR() < this->pitch.getGlobalBounds().position.x)
					player->setBallInGoal(true);
			// left
			if (player->getPlayerCenterPos().x - player->getR() + (player->getStep().x * player->getSpeed()) < this->leftGoal.getGlobalBounds().position.x) {
				player->changeStepDir(-1.f, 1.f);
				player->setNewSpeed(player->getSpeed() * 0.5f);
			}
			// top
			if (player->getPlayerCenterPos().y - player->getR() + (player->getStep().y * player->getSpeed()) < this->leftGoal.getGlobalBounds().position.y) {
				player->changeStepDir(1.f, -1.f);
				player->setNewSpeed(player->getSpeed() * 0.5f);
			}
			// bottom
			else if (player->getPlayerCenterPos().y + player->getR() + (player->getStep().y * player->getSpeed()) > this->leftGoal.getGlobalBounds().position.y + this->leftGoal.getGlobalBounds().size.y) {
				player->changeStepDir(1.f, -1.f);
				player->setNewSpeed(player->getSpeed() * 0.5f);
			}
		}
	}
	// right
	else if (player->getPlayerCenterPos().x + player->getR() + (player->getStep().x * player->getSpeed()) > this->pitch.getGlobalBounds().position.x + this->pitch.getGlobalBounds().size.x) {
		if (!player->isInGoal(this->rightGoal.getGlobalBounds())) {
			player->setNewPos(
				this->pitch.getGlobalBounds().position.x + this->pitch.getGlobalBounds().size.x - player->getR() * 2.f,
				player->getPlayerCenterPos().y - player->getR()
			);
			player->changeStepDir(-1.f, 1.f);
			player->setNewSpeed(player->getSpeed() * 0.9f);
		}
		// collision inside right goal
		else {
			if (player->getColor() == sf::Color::White)
				if (player->getPlayerCenterPos().x - player->getR() > this->pitch.getGlobalBounds().position.x + this->pitch.getGlobalBounds().size.x)
					player->setBallInGoal(true);
			if (player->getPlayerCenterPos().x + player->getR() + (player->getStep().x * player->getSpeed()) > this->rightGoal.getGlobalBounds().position.x + this->rightGoal.getGlobalBounds().size.x) {
				player->changeStepDir(-1.f, 1.f);
				player->setNewSpeed(player->getSpeed() * 0.5f);
			}
			if (player->getPlayerCenterPos().y - player->getR() + (player->getStep().y * player->getSpeed()) < this->rightGoal.getGlobalBounds().position.y) {
				player->changeStepDir(1.f, -1.f);
				player->setNewSpeed(player->getSpeed() * 0.5f);
			}
			else if (player->getPlayerCenterPos().y + player->getR() + (player->getStep().y * player->getSpeed()) > this->rightGoal.getGlobalBounds().position.y + this->rightGoal.getGlobalBounds().size.y) {
				player->changeStepDir(1.f, -1.f);
				player->setNewSpeed(player->getSpeed() * 0.5f);
			}
		}
	}
	// top
	if (player->getPlayerCenterPos().y - player->getR() + (player->getStep().y * player->getSpeed()) < this->pitch.getGlobalBounds().position.y) {
		player->setNewPos(
			player->getPlayerCenterPos().x - player->getR(),
			this->pitch.getGlobalBounds().position.y
		);
		player->changeStepDir(1.f, -1.f);
		player->setNewSpeed(player->getSpeed() * 0.9f);
	}
	// bottom
	else if (player->getPlayerCenterPos().y + player->getR() + (player->getStep().y * player->getSpeed()) > this->pitch.getGlobalBounds().position.y + this->pitch.getGlobalBounds().size.y) {
		player->setNewPos(
			player->getPlayerCenterPos().x - player->getR(),
			this->pitch.getGlobalBounds().position.y + this->pitch.getGlobalBounds().size.y - player->getR() * 2.f
		);
		player->changeStepDir(1.f, -1.f);
		player->setNewSpeed(player->getSpeed() * 0.9f);
	}
}

const sf::FloatRect Pitch::getPitchBounds(std::string side) const
{
	if (side == "l")
		return this->leftGoal.getGlobalBounds();
	else if (side == "r")
		return this->rightGoal.getGlobalBounds();
	else
		return this->pitch.getGlobalBounds();
}

void Pitch::handleIfSomeoneIsInGoal(std::vector<Player> *players, int *blueScore, int *redScore, bool *gameOver)
{
	for (auto &a : *players) {
		if (a.getIsResettingPos()) continue;
		float pax = a.getPlayerCenterPos().x - a.getR();
		float step = 0;
		bool in_goal = false;
		float new_pax = 0;
		// l goal
		if (a.isInGoal(this->leftGoal.getGlobalBounds(), "l")) {
			if (a.getColor() == sf::Color::White) {
				if (a.getBallInGoal()) {
					(*blueScore)++;
					a.setBallInGoal(false);
					// reset all pos
					// game->reInitPlayersPos();
					if (*blueScore == 3)
						*gameOver = true;
					break;
				}
				continue;
			}
			new_pax = this->pitch.getGlobalBounds().position.x;
			step = a.getR() * 2.f;
			in_goal = true;
		}
		// r goal
		else if (a.isInGoal(this->rightGoal.getGlobalBounds(), "r")) {
			if (a.getColor() == sf::Color::White) {
				if (a.getBallInGoal()) {
					(*redScore)++;
					a.setBallInGoal(false);
					// reset all pos
					// game->reInitPlayersPos();
					if (*redScore == 3)
						*gameOver = true;
					break;
				}
				continue;
			}
			new_pax = this->pitch.getGlobalBounds().position.x + this->pitch.getGlobalBounds().size.x - a.getR() * 2.f;
			step = a.getR() * 2.f * -1.f;
			in_goal = true;
		}
		// if yes move player outside of the goal before changing side of the play
		if (in_goal) {
			bool interference = true;
			while (interference) {
				interference = false;
				new_pax += step;
				a.setNewPos(new_pax, a.getPlayerCenterPos().y - a.getR());
				for (auto &b: *players) {
					if (&a == &b) continue;
					if (a.checkCollisionWith(&b)) {
						interference = true;
						break;
					}
				}
			}
			a.setNewPos(pax, a.getPlayerCenterPos().y - a.getR());
			a.setXAfterResetting(new_pax);
			a.setIsResettingPos(true);
		}
	}
}
