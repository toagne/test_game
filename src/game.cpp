#include "Game.hpp"

// Constructor & Desctructor
Game::Game() {
	this->initVariables();
	this->initWindow();
}

Game::~Game() {
	delete this->window;
}

// Private Functions
void Game::initVariables() {
	this->window = nullptr;
}

void Game::initWindow() {
	this->window = new sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "CMake SFML Project");
	this->window->setFramerateLimit(144);
}

// Accessories
const bool Game::getWindowIsOpen() const {
	return this->window->isOpen();
}

// Public functions
void Game::pollEvents()
{
	while (const std::optional event = this->window->pollEvent())
	{
		if (event->is<sf::Event::Closed>())
			this->window->close();
		else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
				this->window->close();
	}
}

void Game::gameUpdate() {
	this->pollEvents();
}

void Game::gameRender() {
	this->window->clear();
	this->window->display();
}

