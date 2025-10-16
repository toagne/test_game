#include "Game.hpp"

// Constructor & Desctructor
Game::Game()
		: window(),
		videoMode(sf::VideoMode::getDesktopMode()),
		selected(nullptr),
		firstTeamPlaying(true),
		playIsOn(false),
		redScore(0),
		blueScore(0),
		scoreText(this->font, "NONE", 50),
		winnerText(this->font, "NONE", 100),
		gameOver(false)
{
	// this->initVariables();
	this->initWindow();
	this->pitch = Pitch(this->videoMode.size);
	this->initPlayers(this->videoMode.size.x, this->videoMode.size.y);

	if (!this->font.openFromFile("/Users/mpellegr/test_game/Font/Cardot-lxq6q.ttf"))
		std::cout << "error loading font";
}

Game::~Game() {
}

void Game::initWindow() {
	this->videoMode = sf::VideoMode::getDesktopMode();

	this->videoMode.size.x = static_cast<int>(this->videoMode.size.x * .9 / 10) * 10;
	this->videoMode.size.y = static_cast<int>(this->videoMode.size.y * .9 / 10) * 10;

	this->window.create(this->videoMode, "CMake SFML Project");
	this->window.setFramerateLimit(60);
}

// Accessories
const bool Game::getWindowIsOpen() const {
	return this->window.isOpen();
}

sf::Vector2f Game::getMousePos() {
	return this->window.mapPixelToCoords(sf::Mouse::getPosition(this->window)); // window is a sf::Window
}

std::vector<sf::Vector2f> Game::getStartingPositions(unsigned int w, unsigned int h)
{
	std::vector<sf::Vector2f> pos;
	float def_x = w / 10.f * 2;
	float def_y = h / 2.f;
	float midL_x = w / 10.f * 3;
	float midL_y = h / 3.f;
	float midR_x = w / 10.f * 3;
	float midR_y = h / 3.f * 2;
	float st_x = w / 10.f * 4;
	float st_y = h / 2.f;

	pos.emplace_back(def_x, def_y);
	pos.emplace_back(w - def_x, h - def_y);

	pos.emplace_back(midL_x, midL_y);
	pos.emplace_back(w - midL_x, h - midL_y);

	pos.emplace_back(midR_x, midR_y);
	pos.emplace_back(w - midR_x, h - midR_y);

	pos.emplace_back(st_x, st_y);
	pos.emplace_back(w - st_x, h - st_y);

	pos.emplace_back(w / 2.f, h / 2.f);
	return pos;
}

void Game::initPlayers(unsigned int w, unsigned int h)
{
	std::vector<sf::Vector2f> positions = this->getStartingPositions(w, h);
	for (size_t i = 0; i < positions.size(); i++)
		this->players.emplace_back(positions[i].x, positions[i].y, i);
}

Player* Game::selectPlayer(sf::Vector2f mouseLocalPosition, bool firstTeamPlaying) {
	for (auto &player : players) {
		if (firstTeamPlaying && player.getIndex() % 2 == 0 && player.getClick(mouseLocalPosition))
			return &player;
		else if (!firstTeamPlaying && player.getIndex() % 2 != 0 && player.getClick(mouseLocalPosition))
			return &player;
	}
	return nullptr;
}

void Game::reInitPlayersPos()
{
	std::vector<sf::Vector2f> positions = this->getStartingPositions(this->videoMode.size.x, this->videoMode.size.y);
	for (size_t i = 0; i < positions.size(); i++) {
		this->players[i].setNewPos(positions[i].x - this->players[i].getR(), positions[i].y - this->players[i].getR());
	}
}

// Public functions
void Game::pollEvents()
{
	while (const std::optional event = this->window.pollEvent())
	{
		if (event->is<sf::Event::Closed>()) {
			this->window.close();
		}
		else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
				this->window.close();
		}
		else if (const auto *buttonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
			if (!this->gameOver && buttonPressed->button == sf::Mouse::Button::Left) {
				this->getMousePos();
				this->selected = this->selectPlayer(this->getMousePos(), firstTeamPlaying);
			}
		}
		else if (const auto *buttonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
			if (!this->gameOver && buttonReleased->button == sf::Mouse::Button::Left) {
				if (this->selected && this->selected->getIsAiming()) {
					this->selected->setIsMoving(true);
					this->selected = nullptr;
					this->playIsOn = true;
				}
				else
					this->selected = nullptr;
			}
		}
	}
}

void Game::gameUpdate() {
	this->pollEvents();

	// get aiming info(direction, speed etc.)
	if (this->selected)
		this->selected->aim(this->getMousePos());

	// check boundaries with the pitch
	for (auto &player: players)
		if (player.getIsMoving())
			this->pitch.checkBoundaries(&player);

	// collision with a player
	size_t n = players.size();
	for (size_t i = 0; i < n; ++i)
		for (size_t j = i + 1; j < n; ++j)
			if (!players[i].getIsResettingPos() && !players[j].getIsResettingPos() && players[i].checkCollisionWith(&players[j]))
				players[i].handleCollisionWith(&players[j]);

	// move players
	size_t moving_counter = 0;
	for (auto &p: players){
		if (p.getIsMoving()) {
			p.playerMoves();
			moving_counter++;
		}
	}

	// checking status between plays - player in goal/ball in goal/game over
	if (!moving_counter && this->playIsOn) {
		int rs = this->redScore;
		int bs = this->blueScore;
		this->pitch.handleIfSomeoneIsInGoal(&players, &this->blueScore, &this->redScore, &this->gameOver);
		// ball in goal - reset positions
		if (rs != this->redScore || bs != this->blueScore)
			this->reInitPlayersPos();
		// reset movement
		size_t resetting_counter = 0;
		for (auto &p: players) {
			if (p.getIsResettingPos()) {
				p.resetPos();
				resetting_counter++;
			}
		}
		// terminate the play and change side
		if (!resetting_counter) {
			this->playIsOn = false;
			this->firstTeamPlaying = !this->firstTeamPlaying;
		}
	}
}

void Game::drawTeam() {
	bool someoneSelected = false;
	for (auto &p : players) {
		if (&p == this->selected) {
			someoneSelected = true;
			break;
		}
	}
	for (auto &player : players) {
		player.drawPlayer(this->window, this->firstTeamPlaying, this->playIsOn, someoneSelected, selected);
	}
}

void Game::drawTexts()
{
	std::stringstream ss;
	ss << this->redScore << " - " << this->blueScore;
	scoreText.setString(ss.str());
	scoreText.setPosition({this->videoMode.size.x / 2.f - (scoreText.getGlobalBounds().size.x / 2.f) , this->videoMode.size.y / 20.f});
	this->window.draw(scoreText);

	if (this->gameOver) {
		if (this->redScore == 3) {
			winnerText.setString("Red team won");
			winnerText.setFillColor(sf::Color::Red);
		}
		if (this->blueScore == 3) {
			winnerText.setString("Blue team won");
			winnerText.setFillColor(sf::Color::Blue);
		}
		winnerText.setPosition({this->videoMode.size.x / 2.f - (winnerText.getGlobalBounds().size.x / 2.f), this->videoMode.size.y / 2.f - (winnerText.getGlobalBounds().size.y / 2.f)});
		this->window.draw(winnerText);
	}
}

void Game::gameRender() {
	this->window.clear();
	// draw pitch
	this->pitch.drawPitch(this->window);

	// draw aiming
	if (this->selected && !this->selected->getIsMoving() && this->selected->getIsAiming()) {
		this->selected->drawAimingShapes(this->window, this->getMousePos());
	}

	// draw players & ball
	if (!this->gameOver)
		this->drawTeam();

	// draw text for score
	this->drawTexts();

	this->window.display();
}